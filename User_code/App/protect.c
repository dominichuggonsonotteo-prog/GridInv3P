

#include "protect.h"  // 包含保护检测相关的头文件

// 保护计数器定义
uint16_t g_dcOvpCnt = 0;      // 直流过压保护计数器
uint16_t g_dcOcpCnt = 0;      // 直流过流保护计数器
uint16_t g_acRmsOvpCnt = 0;   // 交流RMS过压保护计数器
uint16_t g_acRmsOcpCnt = 0;   // 交流RMS过流保护计数器

// 错误标志位清除函数
void Inv3pProtectClear(void)
{
    // 清除所有故障标志位
    g_inv3pLoop.inv3pFaultFlag = 0;
    
    // 复位所有保护计数器
    g_dcOvpCnt = 0;
    g_dcOcpCnt = 0;
    g_acRmsOvpCnt = 0;
    g_acRmsOcpCnt = 0;
    g_uartOverTimeCnt = 0;  // 复位UART超时计数器
}

// 错误检测函数
void ProtectPoll(void)
{
    static uint16_t resetCmd = 0;           // 复位命令状态
    static uint16_t resetCmdDly = 0;        // 复位命令延迟状态
    static uint16_t g_inv3pAdcProtCnt = 0;  // ADC保护计数器
    static uint16_t g_inv3pLoopCntPre = 0;  // 控制环计数器前值
    
    float absVacA, absVacB, absVacC;  // 三相交流电压绝对值
    float absIacA, absIacB, absIacC;  // 三相交流电流绝对值
    float absVdc;                     // 直流电压绝对值

    // 仅在电源开启状态下进行保护检测
    if (POWER_ONOFF == TRUE) {
        // 计算电压电流绝对值
        absVacA = AbsFloat(g_vacRealACorrect);
        absVacB = AbsFloat(g_vacRealBCorrect);
        absVacC = AbsFloat(g_vacRealCCorrect);
        absIacA = AbsFloat(g_iacRealACorrect);
        absIacB = AbsFloat(g_iacRealBCorrect);
        absIacC = AbsFloat(g_iacRealCCorrect);
        absVdc = AbsFloat(g_inv3pVdc.real);
        
        // 执行各种保护检测
        FastProtectDetect(&absVdc, DC_FAST_OVP_THRESHOLD, INV3P_FAST_VDC_OVP);  // 直流快速过压保护
        SlowProtectDetect(&absVdc, DC_SLOW_OVP_THRESHOLD, CNT_100MS_100US, INV3P_SLOW_VDC_OVP, &g_dcOvpCnt);  // 直流慢速过压保护
        
        FastProtectDetect(&absVacA, AC_FAST_OVP_THRESHOLD, INV3P_FAST_AC_OVP);  // A相快速过压保护
        FastProtectDetect(&absVacB, AC_FAST_OVP_THRESHOLD, INV3P_FAST_AC_OVP);  // B相快速过压保护
        FastProtectDetect(&absVacC, AC_FAST_OVP_THRESHOLD, INV3P_FAST_AC_OVP);  // C相快速过压保护
        SlowProtectDetect(&g_vacRmsReport, AC_SLOW_OVP_THRESHOLD, CNT_100MS_100US, INV3P_SLOW_AC_OVP, &g_acRmsOvpCnt);  // 交流慢速过压保护
        
        FastProtectDetect(&absIacA, AC_FAST_OCP_THRESHOLD, INV3P_FAST_AC_OCP);  // A相快速过流保护
        FastProtectDetect(&absIacB, AC_FAST_OCP_THRESHOLD, INV3P_FAST_AC_OCP);  // B相快速过流保护
        FastProtectDetect(&absIacC, AC_FAST_OCP_THRESHOLD, INV3P_FAST_AC_OCP);  // C相快速过流保护
        SlowProtectDetect(&g_iacRmsReport, AC_SLOW_OCP_THRESHOLD, CNT_100MS_100US, INV3P_SLOW_AC_OCP, &g_acRmsOcpCnt);  // 交流慢速过流保护
        
        // UART通信超时保护
        if ((IS_INV3P_ON == TRUE) && (g_uartOverTimeCnt > CNT_3S_10MS)) {
            g_inv3pLoop.inv3pFaultFlag |= INV3P_UART_OVER_TIME;
        }
        
        // ADC门极错误保护检测
        if ((IS_INV3P_ON == TRUE) && (g_inv3pLoopCnt == g_inv3pLoopCntPre) && (g_inv3pLoopCnt != 0)) {
            if (g_inv3pAdcProtCnt < CNT_20MS_100US) {
                g_inv3pAdcProtCnt++;
            } else {
                g_inv3pAdcProtCnt = 0;
                g_inv3pLoop.inv3pFaultFlag |= INV3P_ADC_GATE_ERROR;  // 设置ADC门极错误标志
            }
        } else {
            g_inv3pAdcProtCnt = 0;  // 重置ADC保护计数器
        }
        g_inv3pLoopCntPre = g_inv3pLoopCnt;  // 更新控制环计数器前值
    } else {
        // 电源关闭时复位相关计数器
        g_inv3pAdcProtCnt = 0;
        g_inv3pLoopCnt = 0;
        g_inv3pLoopCntPre = 0;
    }
    
    // 故障处理：关闭PWM，点亮黄灯，开启蜂鸣器
    if (g_inv3pLoop.inv3pFaultFlag != 0) {
        Inv3pPwmOff();   // 关闭PWM输出
        LED_RED_ON;   // 点亮红色指示灯
        BEEP_ON;         // 开启蜂鸣器报警
    } else {
        LED_RED_OFF;  // 关闭红色指示灯
        BEEP_OFF;        // 关闭蜂鸣器
    }

    // 复位命令检测处理
    resetCmdDly = resetCmd;     // 保存前一次复位命令状态
    resetCmd = RESET_EN;        // 读取当前复位命令状态
    
    // 检测复位命令下降沿（当电源关闭时）
    if ((resetCmdDly == 1) && (resetCmd == 0) && ((POWER_ONOFF == FALSE))) {
        Inv3pProtectClear();  // 清除保护状态
    }
}

// 快速保护：当实时值protectParamValue超过保护阈值protectValue后，立刻置位错误标志protectBit
void FastProtectDetect(float * protectParamValue, float protectValue, uint16_t protectBit)
{
    // 检测参数是否超过保护阈值
    if ((*protectParamValue) > protectValue) {
        g_inv3pLoop.inv3pFaultFlag |= protectBit;  // 设置对应的保护标志位
    }
}

// 慢速保护：当实时值protectParamValue超过保护阈值protectValue后，且维持overTime个周期后置位错误标志protectBit
void SlowProtectDetect(float * protectParamValue, float protectValue, uint16_t overTime, uint16_t protectBit, uint16_t * overTimeCnt)
{
    // 参数超过阈值时增加计数器，否则减少（带下限保护）
    if ((*protectParamValue) > protectValue) {
        (*overTimeCnt)++;
    } else if ((*overTimeCnt) > 0) {
        (*overTimeCnt)--;
    }
    
    // 当超时计数超过阈值时触发保护
    if ((*overTimeCnt) > overTime) {
        g_inv3pLoop.inv3pFaultFlag |= protectBit;  // 设置对应的保护标志位
    }
}
