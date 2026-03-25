/****************************************************************************************
  * @file    idle.c
  * @author  PowerX 功率探索
  * @version V1.0.0
  * @date    01-July-2023
  * @淘宝店铺链接：https://gvmu6dghak5o8m0b3kv7j3j8bx49256.taobao.com/shop/view_shop.htm?spm=a1z0k.7628869.0.0.2a5a37deRiWnIl&shop_id=267446508
  * @LegalDeclaration ：本文档内容难免存在Bug，仅限于交流学习，禁止用于任何的商业用途
  * @Copyright   著作权归 “PowerX 功率探索” 所有
*****************************************************************************************/

#include "idle.h"

/* 全局变量定义与详细说明 - 添加单位和使用说明 */
float g_iacDReport = 0.0f;               // 交流电流D轴分量实时值 (单位: A)
float g_iacQReport = 0.0f;               // 交流电流Q轴分量实时值 (单位: A)
float g_vacRmsReport = 0.0f;             // 交流电压有效值实时计算值 (单位: V)
float g_iacRmsReport = 0.0f;             // 交流电流有效值实时计算值 (单位: A)
float g_moduReport = 0.0f;               // 调制比实时值 (范围: 0.0-1.0)
float g_iacDReportFilter = 0.0f;         // 滤波后的交流电流D轴分量 (单位: A)
float g_iacQReportFilter = 0.0f;         // 滤波后的交流电流Q轴分量 (单位: A)
float g_vacRmsReportFilter = 0.0f;       // 滤波后的交流电压有效值 (单位: V)
float g_freqReport = PLL_FREQ_RATED;     // 滤波后的电网频率 (单位: Hz)
 
float g_inv3pIdRef = 0.0f;               // 逆变器D轴电流参考值 (单位: A)
float g_inv3pIqRef = 0.0f;               // 逆变器Q轴电流参考值 (单位: A)
float g_inv3pIdRefRamp = 0.0f;           // 斜坡处理后的D轴电流参考值 (单位: A)
float g_inv3pIqRefRamp = 0.0f;           // 斜坡处理后的Q轴电流参考值 (单位: A)
 
float g_vdcRealFilter = 40.0f;           // 滤波后的直流母线电压 (单位: V)
 
float g_vacRealAZeroBias = 0.0f;         // A相电压零点漂移补偿值 (单位: V)
float g_vacRealBZeroBias = 0.0f;         // B相电压零点漂移补偿值 (单位: V)
float g_vacRealCZeroBias = 0.0f;         // C相电压零点漂移补偿值 (单位: V)
float g_iacRealAZeroBias = 0.0f;         // A相电流零点漂移补偿值 (单位: A)
float g_iacRealBZeroBias = 0.0f;         // B相电流零点漂移补偿值 (单位: A)
float g_iacRealCZeroBias = 0.0f;         // C相电流零点漂移补偿值 (单位: A)
 
uint16_t g_pllEN = 0;                    // 锁相环使能标志: 0=禁用, 1=使能
 
uint16_t g_powerRunTimeSec = 0;          // 逆变器运行时间(秒)
 
uint16_t g_uartOverTimeCnt = 0;          // 串口通信超时计数器 (单位: ms)

 
int16_t AbsInt16(int16_t parameter)
{
    if (parameter < 0) {          // 检查参数是否为负数
        return (0 - parameter);   // 负数取反得到绝对值
    } else {                      // 非负数情况
        return parameter;         // 直接返回原值（正数或零）
    }
}

 
CCMRAM float AbsFloat(float parameter)
{
    if (parameter < 0.0f) {          // 检查参数是否为负数
        return (0.0f - parameter);   // 负数取反得到绝对值
    } else {                         // 非负数情况
        return parameter;            // 直接返回原值（正数或零）
    }
}

 
CCMRAM void LowPassFilter(float *output, float *input, float filterTs)
{
    (*output) = (*output) * filterTs + (*input) * (1 - filterTs);  // 一阶低通滤波计算
}

 
void LedBlink(void)
{
    GpioOutputToggle(GPIOC, GPIO_Pin_13);  // 切换LED状态
}

 
void FanStatusFresh(void)
{
    if (FAN_EN == TRUE) {
        FAN_ON;  // 开启风扇
    } else {
        FAN_OFF; // 关闭风扇
    }
}

 
void ReportFilter(void)
{
    // 计算交流电压有效值（通过αβ分量计算RMS）
    g_vacRmsReport = Sqrt(g_vacReal.paramAlpha * g_vacReal.paramAlpha + 
                          g_vacReal.paramBeta * g_vacReal.paramBeta) * SQRT2_RECI;
    
    // 计算交流电流有效值（通过αβ分量计算RMS）
    g_iacRmsReport = Sqrt(g_iacReal.paramAlpha * g_iacReal.paramAlpha + 
                          g_iacReal.paramBeta * g_iacReal.paramBeta) * SQRT2_RECI;
    
    // 滤波处理关键参数
    LowPassFilter(&g_vacRmsReportFilter, &g_vacRmsReport, 0.99f);        // 交流电压有效值滤波
    LowPassFilter(&g_vdcRealFilter, &INV3P_VDC, 0.9);                    // 直流母线电压滤波
    LowPassFilter(&g_vacRealAZeroBias, &INV3P_VA, 0.9999);               // A相电压零点漂移补偿
    LowPassFilter(&g_vacRealBZeroBias, &INV3P_VB, 0.9999);               // B相电压零点漂移补偿
    LowPassFilter(&g_vacRealCZeroBias, &INV3P_VC, 0.9999);               // C相电压零点漂移补偿
    LowPassFilter(&g_iacRealAZeroBias, &INV3P_IA, 0.9999);               // A相电流零点漂移补偿
    LowPassFilter(&g_iacRealBZeroBias, &INV3P_IB, 0.9999);               // B相电流零点漂移补偿
    LowPassFilter(&g_iacRealCZeroBias, &INV3P_IC, 0.9999);               // C相电流零点漂移补偿
    LowPassFilter(&g_freqReport, &g_pllLoop.freq, 0.98);                 // 电网频率滤波
    
    // 逆变器运行时处理电流数据
    if (IS_INV3P_ON == TRUE) {
        g_moduReport = Sqrt(g_modulateSign.paramAlpha * g_modulateSign.paramAlpha + 
                            g_modulateSign.paramBeta * g_modulateSign.paramBeta);  // 计算调制比
        AlphaBetaToDq(&g_iacReal);                 // 坐标变换：αβ -> DQ（静止坐标系转旋转坐标系）
        g_iacDReport = g_iacReal.paramD;           // 获取D轴电流（有功分量）
        g_iacQReport = g_iacReal.paramQ;           // 获取Q轴电流（无功分量）
        LowPassFilter(&g_iacDReportFilter, &g_iacDReport, 0.99f);  // D轴电流滤波
        LowPassFilter(&g_iacQReportFilter, &g_iacQReport, 0.99f);  // Q轴电流滤波
    } else {
        // 逆变器停止时清零相关参数
        g_moduReport = 0.0f;
        g_iacDReport = 0.0f;
        g_iacQReport = 0.0f;
        g_iacDReportFilter = 0.0f;
        g_iacQReportFilter = 0.0f;
    }
}

 
void RefValueClamp(void)
{
    float iacRefTemp; // 临时存储电流参考值幅值
    
    // 计算电流参考值幅值（矢量模长）
    iacRefTemp = Sqrt(INV3P_ID_REF * INV3P_ID_REF + \
                      INV3P_IQ_REF * INV3P_IQ_REF);
    
    // 如果电流参考值在安全范围内，直接使用
    if (iacRefTemp < (10.0f)) {
        g_inv3pIdRef = INV3P_ID_REF;
        g_inv3pIqRef = INV3P_IQ_REF;
    }
}

 
void ParamSoftStart(float *output, float *input, float rampParam)
{
    // 输出值小于目标值时增加
    if (((*output) + rampParam) < (*input)) {
        (*output) += rampParam;
    }
    // 输出值大于目标值时减小
    if (((*output) - rampParam) > (*input)) {
        (*output) -= rampParam;
    }
    // 达到目标范围时直接设为目标值
    if (((*output) + rampParam) >= (*input) &&
        ((*output) - rampParam) <= (*input)) {
        (*output) = (*input);
    }
}

 
void Inv3pStatusFresh(void)
{
    static uint16_t pllLoopOkCnt = 0; // PLL锁定计数器

    // 设置逆变器关闭标志
    g_inv3pLoop.inv3pStatusFlag |= INV3P_CLOSE_FLAG;
    
    // 逆变器运行状态检测
    if (IS_INV3P_ON == TRUE) {
        // 检查实际电流与参考值的偏差
        if ((AbsFloat(g_inv3pIdRefRamp - g_iacDReportFilter) < (0.1f)) && 
            (AbsFloat(g_inv3pIqRefRamp - g_iacQReportFilter) < (0.1f))) {
            g_inv3pLoop.inv3pStatusFlag |= INV3P_POWER_OK_FLAG;  // 功率输出正常
        } else {
            g_inv3pLoop.inv3pStatusFlag &= (~INV3P_POWER_OK_FLAG); // 功率异常
        }
    } else {
        // 逆变器停止时清除功率正常标志
        g_inv3pLoop.inv3pStatusFlag &= (~INV3P_POWER_OK_FLAG);
    }

    // 锁相环状态检测
    if (g_pllEN == TRUE) {
        // 检测电网频率是否在额定值附近
        if (AbsFloat(g_freqReport - PLL_FREQ_RATED) < (0.2f)) {
            pllLoopOkCnt++;  // 增加锁定计数
        } else {
            pllLoopOkCnt = 0; // 频率超限时重置计数器
        }
        
        // 连续10次检测正常则认为PLL锁定
        if (pllLoopOkCnt >= 10) {
            pllLoopOkCnt = 10; // 防止计数器溢出
            g_inv3pLoop.inv3pStatusFlag |= INV3P_PLL_OK_FLAG;  // 设置PLL锁定标志
        } else {
            g_inv3pLoop.inv3pStatusFlag &= (~INV3P_PLL_OK_FLAG); // 清除PLL锁定标志
        }
    } else {
        // PLL禁用时清除相关标志
        pllLoopOkCnt = 0;
        g_inv3pLoop.inv3pStatusFlag &= (~INV3P_PLL_OK_FLAG);
    }
}

 
void Inv3pOnoffProcess(void)
{
    static uint16_t vdcClampOk = 0;      // 直流电压钳位状态
    static uint32_t powerStartTimeMs = 0; // 运行开始时间

    // 检测直流电压是否达到启动阈值
    if (g_vdcRealFilter > (g_vacRmsReportFilter * SQRT2x2 + (5.0f))) {
        vdcClampOk = 1;  // 电压达标
    } else if (g_vdcRealFilter < (g_vacRmsReportFilter * SQRT2x2 + (3.0f))) {
        vdcClampOk = 0;  // 电压不足
    }

    // 逆变器启动条件检查
    if ((POWER_ONOFF == TRUE) && (g_pllEN == TRUE) && (g_inv3pLoop.inv3pFaultFlag == 0) && \
        (vdcClampOk == 1) && (IS_PLL_OK == TRUE)) {
        // 逆变器当前未运行则启动
        if (IS_INV3P_ON == FALSE) {
            Inv3pPwmOn();  // 开启PWM输出
            g_inv3pLoop.inv3pStatusFlag |= INV3P_ON_OFF_FLAG; // 设置运行标志
        } else {
            LED_BLUE_ON;  // 运行状态指示灯
            // DQ轴电流参考值软启动
            ParamSoftStart(&g_inv3pIdRefRamp, &g_inv3pIdRef, (0.02f));
            ParamSoftStart(&g_inv3pIqRefRamp, &g_inv3pIqRef, (0.02f));
        }
        // 计算运行时间（秒）
        g_powerRunTimeSec = (uint16_t) ((g_systemTimeBaseMs - powerStartTimeMs) >> 10);
    } else {
        // 关闭逆变器
        Inv3pPwmOff();  // 关闭PWM输出
        g_inv3pLoop.inv3pStatusFlag &= (~INV3P_ON_OFF_FLAG); // 清除运行标志
        LED_BLUE_OFF;   // 关闭指示灯
        
        // 清除控制环路状态
        Inv3pLoopClear();
        Inv3pHarmLoopClear();
        
        // 重置参考值
        g_inv3pIdRefRamp = (0.0f);
        g_inv3pIqRefRamp = (0.0f);
        
        // 重置PLL使能状态
        g_pllEN = PLL_EN;
        
        // 重置运行时间
        powerStartTimeMs = g_systemTimeBaseMs;
        g_powerRunTimeSec = 0;
    }
}

 
void SendDataToPc(void)
{
    uint16_t *ptr = (uint16_t *) (&g_sendToPC); // 数据指针

    // 设置数据包头
    g_sendToPC.head = UART_HEAD;
    
    // 填充实时数据（转换为整型并缩放）
    g_sendToPC.inv3pVdc = (uint16_t) (AbsFloat(g_vdcRealFilter) * (100.0f));     // 直流电压 (x100)
    g_sendToPC.inv3pVacRms = (uint16_t) (g_vacRmsReportFilter * (100.0f));       // 交流电压有效值 (x100)
    g_sendToPC.inv3pId = (uint16_t) ((g_iacDReportFilter + (100.0f)) * (100.0f)); // D轴电流 (偏移+100后x100)
    g_sendToPC.inv3pIq = (uint16_t) ((g_iacQReportFilter + (100.0f)) * (100.0f)); // Q轴电流 (偏移+100后x100)
    g_sendToPC.inv3pFreq = (uint16_t) (g_freqReport * (100.0f));                 // 频率 (x100)
    g_sendToPC.inv3pModu = (uint16_t) (g_moduReport * (10000.0f));               // 调制比 (x10000)
    
    // 状态标志
    g_sendToPC.inv3pFaultFlag = g_inv3pLoop.inv3pFaultFlag; // 故障标志
    g_sendToPC.inv3pStatusFlag = g_inv3pLoop.inv3pStatusFlag; // 状态标志
    
    // 参考值
    g_sendToPC.readIdRef = (uint16_t) ((g_inv3pIdRef + (100.0f)) * (100.0f)); // D轴参考电流
    g_sendToPC.readIqRef = (uint16_t) ((g_inv3pIqRef + (100.0f)) * (100.0f)); // Q轴参考电流
    g_sendToPC.readVdcRef = 0; // 预留
    g_sendToPC.readModuRef = 0; // 预留
    
    // 运行时间
    g_sendToPC.powerRunTimeSec = g_powerRunTimeSec; // 运行时间(秒)

    // 计算异或校验
    g_sendToPC.xorVertify = 0;
    for (uint16_t i = 1; i < 14; i++) {
        g_sendToPC.xorVertify ^= (uint16_t) (*(ptr + i)); // 逐字异或
    }

    // 配置DMA发送
    DMA_Cmd(DMA1_Channel2, DISABLE); // 禁用DMA
    DMA_SetCurrDataCounter(DMA1_Channel2, sizeof(g_sendToPC)); // 设置数据长度
    DMA_Cmd(DMA1_Channel2, ENABLE); // 启用DMA传输
}

 
void ReceiveDataFromPC(void)
{
    uint16_t *ptr = (uint16_t *) (&g_cmdFromPCTemp); // 接收缓冲区指针
    uint16_t rxXorVertify; // 本地计算的校验值

    // 检查DMA传输完成标志
    if (DMA_GetFlagStatus(DMA1_FLAG_TC3)) {
        // 验证数据包完整性和包头
        if ((DMA_GetCurrDataCounter(DMA1_Channel3) == sizeof(g_cmdFromPCTemp)) && 
            (g_cmdFromPCTemp.head == UART_HEAD)) {
            // 计算校验和
            rxXorVertify = 0;
            for (uint16_t i = 1; i < 10; i++) {
                rxXorVertify ^= (uint16_t) (*(ptr + i)); // 逐字异或
            }
            // 校验通过则处理数据
            if (g_cmdFromPCTemp.xorVertify == rxXorVertify) {
                g_cmdFromPC = g_cmdFromPCTemp; // 保存有效数据
                
                // D轴电流参考值限幅
                if (g_cmdFromPC.idRef >= MAX_ID_REF_x100) {
                    g_cmdFromPC.idRef = MAX_ID_REF_x100;
                } else if (g_cmdFromPC.idRef <= MIN_ID_REF_x100) {
                    g_cmdFromPC.idRef = MIN_ID_REF_x100;
                }
                // Q轴电流参考值限幅
                if (g_cmdFromPC.iqRef >= MAX_IQ_REF_x100) {
                    g_cmdFromPC.iqRef = MAX_IQ_REF_x100;
                } else if (g_cmdFromPC.iqRef <= MIN_IQ_REF_x100) {
                    g_cmdFromPC.iqRef = MIN_IQ_REF_x100;
                }
                g_uartOverTimeCnt = 0; // 重置通信超时计数器
            }
        }
        DMA_ClearFlag(DMA1_FLAG_TC3); // 清除DMA标志
    }
    g_uartOverTimeCnt++; // 增加超时计数
}

 
void OledFresh(void)
{
    // 变量用于整数和小数部分分离
    uint16_t vdcInteger, vdcFraction;       // 直流电压整数和小数部分
    uint16_t vacRmsInteger, vacRmsFraction; // 交流电压整数和小数部分
    uint16_t iacDInteger, iacDFraction;     // D轴电流整数和小数部分
    uint16_t iacQInteger, iacQFraction;     // Q轴电流整数和小数部分

    // 分离各物理量的整数和小数部分
    vdcInteger = (uint16_t) AbsFloat(g_vdcRealFilter);
    vdcFraction = (uint16_t) ((AbsFloat(g_vdcRealFilter) - vdcInteger) * 100);
    vacRmsInteger = (uint16_t) AbsFloat(g_vacRmsReportFilter);
    vacRmsFraction = (uint16_t) ((AbsFloat(g_vacRmsReportFilter) - vacRmsInteger) * 100);
    iacDInteger = (uint16_t) AbsFloat(g_iacDReportFilter);
    iacDFraction = (uint16_t) ((AbsFloat(g_iacDReportFilter) - iacDInteger) * 100);
    iacQInteger = (uint16_t) AbsFloat(g_iacQReportFilter);
    iacQFraction = (uint16_t) ((AbsFloat(g_iacQReportFilter) - iacQInteger) * 100);

    // 显示直流电压值
    OledShowString(0, 0, "Vdc:", 16); // 标签
    if (g_vdcRealFilter < (0.0f)) {   // 负值显示负号
        OledShowString(40, 0, "-", 16);
    } else {                          // 正值显示空格
        OledShowString(40, 0, " ", 16);
    }
    OledShowNum(48, 0, vdcInteger, 3, 16); // 整数部分
    OledShowString(72, 0, ".", 16);        // 小数点
    if (vdcFraction < 10) {                // 小数部分补零
        OledShowNum(80, 0, 0, 1, 16);
        OledShowNum(88, 0, vdcFraction, 1, 16);
    } else {
        OledShowNum(80, 0, vdcFraction, 2, 16);
    }
    OledShowString(96, 0, "V", 16); // 单位

    // 显示交流电压有效值（格式同直流电压）
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

    // 显示D轴电流值（格式同电压）
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

    // 显示Q轴电流值（格式同D轴电流）
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
