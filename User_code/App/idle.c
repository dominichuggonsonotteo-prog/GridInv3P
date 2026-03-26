#include "idle.h"

// Inv3p的Vac和Iac与modu值上报变量，对实际值大滤波后上报显示
float g_iacDReport = (0.0f);
float g_iacQReport = (0.0f);
float g_vacRmsReport = (0.0f);
float g_iacRmsReport = (0.0f);
float g_moduReport = (0.0f);
float g_iacDReportFilter = (0.0f);
float g_iacQReportFilter = (0.0f);
float g_vacRmsReportFilter = (0.0f);
float g_freqReport = PLL_FREQ_RATED;

// Inv3p的闭环电压电流指令值
float g_inv3pIdRef = (0.0f);
float g_inv3pIqRef = (0.0f);
float g_inv3pIdRefRamp = (0.0f);
float g_inv3pIqRefRamp = (0.0f);

// Inv3p DC输入电压滤波值，默认40V，用于输入电压前馈
float g_vdcRealFilter = (40.0f);

// Inv3p AC电压电流滤波，用于校准零偏
float g_vacRealAZeroBias = (0.0f);
float g_vacRealBZeroBias = (0.0f);
float g_vacRealCZeroBias = (0.0f);
float g_iacRealAZeroBias = (0.0f);
float g_iacRealBZeroBias = (0.0f);
float g_iacRealCZeroBias = (0.0f);

// 状态标志锁存
uint16_t g_pllEN = 0;

// 开机连续运行时间计数，关机复位，单位s
uint16_t g_powerRunTimeSec = 0;

// 串口通信超时计时
uint16_t g_uartOverTimeCnt = 0;

static uint16_t EncodeSignedValueX100(float value)
{
    int32_t scaledValue = (int32_t) (value * (100.0f));
    CLAMP(scaledValue, 32767, -32768);
    return (uint16_t) ((int16_t) scaledValue);
}

static uint16_t EncodeUnsignedValueX10000(float value)
{
    if (value < (0.0f)) {
        value = (0.0f);
    } else if (value > (6.5535f)) {
        value = (6.5535f);
    }

    return (uint16_t) (value * (10000.0f));
}

// 整数求绝对值函数
int16_t AbsInt16(int16_t parameter)
{
    if (parameter < 0) {
        return (0 - parameter);
    } else {
        return parameter;
    }
}

// 浮点数求绝对值函数
CCMRAM float AbsFloat(float parameter)
{
    if (parameter < (0.0f)) {
        return ((0.0f) - parameter);
    } else {
        return parameter;
    }
}

// 一阶低通滤波函数
CCMRAM void LowPassFilter(float *output, float *input, float filterTs)
{
    (*output) = (*output) * filterTs + (*input) * (1 - filterTs);
}

// 绿灯LED狗灯翻转函数
void LedBlink(void)
{
    GpioOutputToggle(GPIOC, GPIO_Pin_13);
}

// 风扇状态刷新函数
void FanStatusFresh(void)
{
    if (FAN_EN == TRUE) {
        FAN_ON;
    } else {
        FAN_OFF;
    }
}

// Vac和Iac上报滤波函数
void ReportFilter(void)
{
    // 计算AC电压的RMS值
    g_vacRmsReport = Sqrt(g_vacReal.paramAlpha * g_vacReal.paramAlpha + \
    g_vacReal.paramBeta * g_vacReal.paramBeta) * SQRT2_RECI;
    
    // 计算AC电流的RMS值
    g_iacRmsReport = Sqrt(g_iacReal.paramAlpha * g_iacReal.paramAlpha + \
    g_iacReal.paramBeta * g_iacReal.paramBeta) * SQRT2_RECI;
    
    // AC电压RMS值低通滤波后用于OLED显示
    LowPassFilter(&g_vacRmsReportFilter, &g_vacRmsReport, (0.99f));
    
    // 输入电压滤波，用于输入前馈计算
    LowPassFilter(&g_vdcRealFilter, &INV3P_VDC, (0.9));
    
    // 交流电压电流滤波，用于校准零偏
    LowPassFilter(&g_vacRealAZeroBias, &INV3P_VA, (0.9999));
    LowPassFilter(&g_vacRealBZeroBias, &INV3P_VB, (0.9999));
    LowPassFilter(&g_vacRealCZeroBias, &INV3P_VC, (0.9999));
    LowPassFilter(&g_iacRealAZeroBias, &INV3P_IA, (0.9999));
    LowPassFilter(&g_iacRealBZeroBias, &INV3P_IB, (0.9999));
    LowPassFilter(&g_iacRealCZeroBias, &INV3P_IC, (0.9999));
    
    // 锁相环输出频率滤波
    LowPassFilter(&g_freqReport, &g_pllLoop.freq, (0.98));
    
    // 电源开机后上报调制比modulate， AC电流的d轴和q轴分量
    if (IS_INV3P_ON == TRUE) {
        g_moduReport = Sqrt(g_modulateSign.paramAlpha * g_modulateSign.paramAlpha + \
        g_modulateSign.paramBeta * g_modulateSign.paramBeta);
        
        AlphaBetaToDq(&g_iacReal);
        g_iacDReport = g_iacReal.paramD;
        g_iacQReport = g_iacReal.paramQ;
        
        LowPassFilter(&g_iacDReportFilter, &g_iacDReport, (0.99f));
        LowPassFilter(&g_iacQReportFilter, &g_iacQReport, (0.99f));
    } else { // 如果未开机，相关变量上报0
        g_moduReport = (0.0f);
        g_iacDReport = (0.0f);
        g_iacQReport = (0.0f);
        g_iacDReportFilter = (0.0f);
        g_iacQReportFilter = (0.0f);
    }
}

// 电压电流指令值的限幅
void RefValueClamp(void)
{
    float iacRefTemp;
    
    // 计算dq轴电流指令合成后的最大峰值
    iacRefTemp = Sqrt(INV3P_ID_REF * INV3P_ID_REF + \
    INV3P_IQ_REF * INV3P_IQ_REF);
    
    // 限制AC电流指令最大RMS值不超过7A
    if (iacRefTemp < (10.0f)) {
        g_inv3pIdRef = INV3P_ID_REF;
        g_inv3pIqRef = INV3P_IQ_REF;
    }
}

// 指令值缓升函数
// 当input阶跃变化后， output以rampParam的步长缓慢爬升接近input
void ParamSoftStart(float *output, float *input, float rampParam)
{
    if (((*output) + rampParam) < (*input)) {
        (*output) += rampParam;
    }
    if (((*output) - rampParam) > (*input)) {
        (*output) -= rampParam;
    }
    if (((*output) + rampParam) >= (*input) &&
        ((*output) - rampParam) <= (*input)) {
        (*output) = (*input);
    }
}


// 电源环路稳定状态位更新函数
void Inv3pStatusFresh(void)
{
    static uint16_t pllLoopOkCnt = 0;

    if (PLL_EN == TRUE) {
        g_inv3pLoop.inv3pStatusFlag |= INV3P_PLL_CMD_FLAG;
    } else {
        g_inv3pLoop.inv3pStatusFlag &= (~INV3P_PLL_CMD_FLAG);
    }

    if (g_pllEN == TRUE) {
        g_inv3pLoop.inv3pStatusFlag |= INV3P_PLL_EN_FLAG;
    } else {
        g_inv3pLoop.inv3pStatusFlag &= (~INV3P_PLL_EN_FLAG);
    }

    g_inv3pLoop.inv3pStatusFlag |= INV3P_CLOSE_FLAG;
    
    //电源在开机状态下才更新环路稳定状态位
		if (IS_INV3P_ON == TRUE) {  //IS_INV3P_ON是上位机发送的控制bit指令
        // 电流环误差<0.1A，则代表环路已经稳定， INV3P_POWER_OK_FLAG标志置位
        if ((AbsFloat(g_inv3pIdRefRamp - g_iacDReportFilter) < (0.1f)) && (AbsFloat(g_inv3pIqRefRamp - g_iacQReportFilter) < (0.1f))) {
            g_inv3pLoop.inv3pStatusFlag |= INV3P_POWER_OK_FLAG;
        } else {
            g_inv3pLoop.inv3pStatusFlag &= (~INV3P_POWER_OK_FLAG);
        }
    } else {
        // 关机状态下环路稳定状态位清零
        g_inv3pLoop.inv3pStatusFlag &= (~INV3P_POWER_OK_FLAG);
    }
    
    if (g_pllEN == TRUE) { // 如果PLL使能标志位置位 g_pllEN被置1的条件就是停止发脉冲
        // 如果PLL的输出频率误差 < 0.2Hz，则环路稳定计数器计数，否则清零
        if (AbsFloat(g_freqReport - PLL_FREQ_RATED) < (0.2f)) {
            pllLoopOkCnt++;
        } else {
            pllLoopOkCnt = 0;
        }
        
        // 当PLL环路稳定计数器计数>10，表示PLL环路稳定， PLL稳定标志位置位
        if (pllLoopOkCnt >= 10) {
            pllLoopOkCnt = 10;
            g_inv3pLoop.inv3pStatusFlag |= INV3P_PLL_OK_FLAG;
        } else {
            g_inv3pLoop.inv3pStatusFlag &= (~INV3P_PLL_OK_FLAG);
        }
    } else { // 如果不使能PLL工作
        // PLL环路稳定计数器清零
        pllLoopOkCnt = 0;
        // PLL环路稳定标志位清零
        g_inv3pLoop.inv3pStatusFlag &= (~INV3P_PLL_OK_FLAG);
    }
}

// 3inv的开关机处理函数
void Inv3pOnoffProcess(void)
{
    static uint16_t vdcClampOk = 0;
    static uint32_t powerStartTimeMs = 0;
    
    // 判断DC电压是否满足开机要求
    if (g_vdcRealFilter > (g_vacRmsReportFilter * SQRT2x2 + (5.0f))) {
        vdcClampOk = 1;
    } else if (g_vdcRealFilter < (g_vacRmsReportFilter * SQRT2x2 + (3.0f))) {
        vdcClampOk = 0;
    }
    
    // 如果接收到开机指令/无故障发生/DC电压满足要求/下位机上报数据-PLL锁相环稳定，则执行开机逻辑   //POWER_ONOFF是QT发过来的bit控制位
    if ((POWER_ONOFF == TRUE) && (g_pllEN == TRUE) && (g_inv3pLoop.inv3pFaultFlag == 0) && \
        (vdcClampOk == 1) && (IS_PLL_OK == TRUE)) {
        // 如果当前为关机状态，则准备开机
        if (IS_INV3P_ON == FALSE) {
            // 开启PWM输出
            Inv3pPwmOn();
            // 电源开机标志位置位
            g_inv3pLoop.inv3pStatusFlag |= INV3P_ON_OFF_FLAG;
        } else {
            // 开机后电源运行指示灯亮起
            LED_ORANGE_ON;
            // AC current reference ramp runs in the 200 us slow task.
            // Keep the lower-side ramp intentionally slow so the inverter
            // can absorb large host-side steps safely.
            ParamSoftStart(&g_inv3pIdRefRamp, &g_inv3pIdRef, INV3P_REF_RAMP_STEP);
            ParamSoftStart(&g_inv3pIqRefRamp, &g_inv3pIqRef, INV3P_REF_RAMP_STEP);
        }
        // 功率连续输出计时，右移10位近似除以1000
        g_powerRunTimeSec = (uint16_t) ((g_systemTimeBaseMs - powerStartTimeMs) >> 10);
    } else { // 如果收到关机指令或故障发生或锁相环不稳定
        // 关闭PWM输出
        Inv3pPwmOff();
        // 电源开机标志位复位
        g_inv3pLoop.inv3pStatusFlag &= (~INV3P_ON_OFF_FLAG);
        // 电源运行指示灯熄灭
        LED_ORANGE_OFF;
        // 闭环PI控制器复位
        Inv3pLoopClear();
        Inv3pHarmLoopClear();
        // 直流电压与交流电流指令的缓启值复位
        g_inv3pIdRefRamp = (0.0f);
        g_inv3pIqRefRamp = (0.0f);
        // 运行状态标志位刷新，开机后锁存
        g_pllEN = PLL_EN;
        // 功率运行时间计数变量复位
        powerStartTimeMs = g_systemTimeBaseMs;
        g_powerRunTimeSec = 0;
    }
}

// 电源参数串口上报函数
void SendDataToPc(void)
{
    uint16_t *ptr = (uint16_t *) (&g_sendToPC);
    float modulateMagnitude = 0.0f;
    g_sendToPC.head = UART_HEAD;
    
    // 变量的上报值为*100后上报
    g_sendToPC.inv3pVdc = (uint16_t) (AbsFloat(g_vdcRealFilter) * (100.0f));
    g_sendToPC.inv3pVacRms = (uint16_t) (g_vacRmsReportFilter * (100.0f));
    g_sendToPC.inv3pId = (uint16_t) ((g_iacDReportFilter + (100.0f)) * (100.0f));
    g_sendToPC.inv3pIq = (uint16_t) ((g_iacQReportFilter + (100.0f)) * (100.0f));
    // 直接上报 PLL 原始输出频率，便于观察锁相环是否真的在动态跟踪
    g_sendToPC.inv3pFreq = (uint16_t) (g_pllLoop.freq * (100.0f));
    // 临时复用调制比字段，上报下位机实际接收并通过校验的控制字
    g_sendToPC.inv3pModu = g_cmdFromPC.pcCmdBit;
    
    if ((g_inv3pLoop.compareCntA <= MIN_COMPARE) || (g_inv3pLoop.compareCntA >= MAX_COMPARE)) {
        g_inv3pLoop.inv3pStatusFlag |= INV3P_PWM_A_CLAMP_FLAG;
    } else {
        g_inv3pLoop.inv3pStatusFlag &= (~INV3P_PWM_A_CLAMP_FLAG);
    }

    if ((g_inv3pLoop.compareCntB <= MIN_COMPARE) || (g_inv3pLoop.compareCntB >= MAX_COMPARE)) {
        g_inv3pLoop.inv3pStatusFlag |= INV3P_PWM_B_CLAMP_FLAG;
    } else {
        g_inv3pLoop.inv3pStatusFlag &= (~INV3P_PWM_B_CLAMP_FLAG);
    }

    if ((g_inv3pLoop.compareCntC <= MIN_COMPARE) || (g_inv3pLoop.compareCntC >= MAX_COMPARE)) {
        g_inv3pLoop.inv3pStatusFlag |= INV3P_PWM_C_CLAMP_FLAG;
    } else {
        g_inv3pLoop.inv3pStatusFlag &= (~INV3P_PWM_C_CLAMP_FLAG);
    }

    // 电源故障和运行状态标志位上报
    g_sendToPC.inv3pFaultFlag = g_inv3pLoop.inv3pFaultFlag;
    g_sendToPC.inv3pStatusFlag = g_inv3pLoop.inv3pStatusFlag;
    
    // 电源指令值回读
    // Read back the ramped reference that is actually entering the control loop.
    g_sendToPC.readIdRef = (uint16_t) ((g_inv3pIdRefRamp + (100.0f)) * (100.0f));
    g_sendToPC.readIqRef = (uint16_t) ((g_inv3pIqRefRamp + (100.0f)) * (100.0f));
    // 临时复用保留回读位上报正无功异常诊断量
    // readVdcRef: α轴电流环输出，使用有符号 int16 编码，缩放 100 倍
    g_sendToPC.readVdcRef = EncodeSignedValueX100(g_inv3pLoop.acCurrAlphaLoop.output);
    // readModuRef: β轴电流环输出，使用有符号 int16 编码，缩放 100 倍
    g_sendToPC.readModuRef = EncodeSignedValueX100(g_inv3pLoop.acCurrBetaLoop.output);

    modulateMagnitude = Sqrt(g_modulateSign.paramAlpha * g_modulateSign.paramAlpha + \
                             g_modulateSign.paramBeta * g_modulateSign.paramBeta);
    // powerRunTimeSec: 调制度矢量幅值，缩放 10000 倍，便于观察是否逼近调制极限
    g_sendToPC.powerRunTimeSec = EncodeUnsignedValueX10000(modulateMagnitude);
    g_sendToPC.compareCntA = (uint16_t) (g_inv3pLoop.compareCntA);
    g_sendToPC.compareCntB = (uint16_t) (g_inv3pLoop.compareCntB);
    g_sendToPC.compareCntC = (uint16_t) (g_inv3pLoop.compareCntC);
    g_sendToPC.phaseCurrA = EncodeSignedValueX100(g_iacRealACorrect);
    g_sendToPC.phaseCurrB = EncodeSignedValueX100(g_iacRealBCorrect);
    g_sendToPC.phaseCurrC = EncodeSignedValueX100(g_iacRealCCorrect);
    
    // 上报上位机的数据加入异或校验
    g_sendToPC.xorVertify = 0;
    for (uint16_t i = 1; i < (sizeof(g_sendToPC) / sizeof(uint16_t)) - 1; i++) {
        g_sendToPC.xorVertify ^= (uint16_t) (*(ptr + i));
    }
    
    DMA_Cmd(DMA1_Channel2, DISABLE);
    DMA_SetCurrDataCounter(DMA1_Channel2, sizeof(g_sendToPC));
    DMA_Cmd(DMA1_Channel2, ENABLE);
}

// 电源串口接收指令函数
void ReceiveDataFromPC(void)
{
    uint16_t *ptr = (uint16_t *) (&g_cmdFromPCTemp);
    uint16_t rxXorVertify;
    
    if (DMA_GetFlagStatus(DMA1_FLAG_TC3)) {
        // 当串口接收数据长度与指令结构体长度相同时，才接收数据
        if ((DMA_GetCurrDataCounter(DMA1_Channel3) == sizeof(g_cmdFromPCTemp)) && (g_cmdFromPCTemp.head == UART_HEAD)) {
            // 接受上位机下发的数据要进行异或校验
            rxXorVertify = 0;
            for (uint16_t i = 1; i < 10; i++) {
                rxXorVertify ^= (uint16_t) (*(ptr + i));
            }
            
            if (g_cmdFromPCTemp.xorVertify == rxXorVertify) {
                g_cmdFromPC = g_cmdFromPCTemp;
                
                // 如果接收到的指令值进行最大最小嵌位
                if (g_cmdFromPC.idRef >= MAX_ID_REF_x100) {
                    g_cmdFromPC.idRef = MAX_ID_REF_x100;
                } else if (g_cmdFromPC.idRef <= MIN_ID_REF_x100) {
                    g_cmdFromPC.idRef = MIN_ID_REF_x100;
                }
                
                if (g_cmdFromPC.iqRef >= MAX_IQ_REF_x100) {
                    g_cmdFromPC.iqRef = MAX_IQ_REF_x100;
                } else if (g_cmdFromPC.iqRef <= MIN_IQ_REF_x100) {
                    g_cmdFromPC.iqRef = MIN_IQ_REF_x100;
                }
                
                g_uartOverTimeCnt = 0;
            }
        }
        DMA_ClearFlag(DMA1_FLAG_TC3);
    }
    g_uartOverTimeCnt++;
}

// OLED显示刷新函数
void OledFresh(void)
{
    // OLED共4行，从上到下分别显示： Vdc/Vac/Id/Iq
    uint16_t vdcInteger, vdcFraction;
    uint16_t vacRmsInteger, vacRmsFraction;
    uint16_t iacDInteger, iacDFraction;
    uint16_t iacQInteger, iacQFraction;
    
    vdcInteger = (uint16_t) AbsFloat(g_vdcRealFilter);
    vdcFraction = (uint16_t) ((AbsFloat(g_vdcRealFilter) - vdcInteger) * 100);
    
    vacRmsInteger = (uint16_t) AbsFloat(g_vacRmsReportFilter);
    vacRmsFraction = (uint16_t) ((AbsFloat(g_vacRmsReportFilter) - vacRmsInteger) * 100);
    
    iacDInteger = (uint16_t) AbsFloat(g_iacDReportFilter);
    iacDFraction = (uint16_t) ((AbsFloat(g_iacDReportFilter) - iacDInteger) * 100);
    
    iacQInteger = (uint16_t) AbsFloat(g_iacQReportFilter);
    iacQFraction = (uint16_t) ((AbsFloat(g_iacQReportFilter) - iacQInteger) * 100);
    
    // Vdc的显示
    OledShowString(0, 0, "Vdc:", 16);
    if (g_vdcRealFilter < (0.0f)) {
        OledShowString(40, 0, "-", 16);
    } else {
        OledShowString(40, 0, " ", 16);
    }
    OledShowNum(48, 0, vdcInteger, 3, 16);
    OledShowString(72, 0, ".", 16);
    if (vdcFraction < 10) {
        OledShowNum(80, 0, 0, 1, 16);
        OledShowNum(88, 0, vdcFraction, 1, 16);
    } else {
        OledShowNum(80, 0, vdcFraction, 2, 16);
    }
    OledShowString(96, 0, "V", 16);
    
    // Vac的显示
    OledShowString(0, 2, "Vac:", 16);
    if (g_vacRmsReportFilter < (0.0f)) {
        OledShowString(40, 2, "-", 16);
    } else {
        OledShowString(40, 2, " ", 16);
    }
    OledShowNum(48, 2, vacRmsInteger, 3, 16);
    OledShowString(72, 2, ".", 16);
    if (vacRmsFraction < 10) {
        OledShowNum(80, 2, 0, 1, 16);
        OledShowNum(88, 2, vacRmsFraction, 1, 16);
    } else {
        OledShowNum(80, 2, vacRmsFraction, 2, 16);
    }
    OledShowString(96, 2, "V", 16);
    
    // Id的显示
    OledShowString(0, 4, "Id :", 16);
    if (g_iacDReportFilter < (0.0f)) {
        OledShowString(40, 4, "-", 16);
    } else {
        OledShowString(40, 4, " ", 16);
    }
    OledShowNum(48, 4, iacDInteger, 3, 16);
    OledShowString(72, 4, ".", 16);
    if (iacDFraction < 10) {
        OledShowNum(80, 4, 0, 1, 16);
        OledShowNum(88, 4, iacDFraction, 1, 16);
    } else {
        OledShowNum(80, 4, iacDFraction, 2, 16);
    }
    OledShowString(96, 4, "A", 16);
    
    // Iq的显示
    OledShowString(0, 6, "Iq :", 16);
    if (g_iacQReportFilter < (0.0f)) {
        OledShowString(40, 6, "-", 16);
    } else {
        OledShowString(40, 6, " ", 16);
    }
    OledShowNum(48, 6, iacQInteger, 3, 16);
    OledShowString(72, 6, ".", 16);
    if (iacQFraction < 10) {
        OledShowNum(80, 6, 0, 1, 16);
        OledShowNum(88, 6, iacQFraction, 1, 16);
    } else {
        OledShowNum(80, 6, iacQFraction, 2, 16);
    }
    OledShowString(96, 6, "A", 16);
}
