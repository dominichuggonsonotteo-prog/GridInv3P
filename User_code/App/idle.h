#ifndef __IDLE_H__
#define __IDLE_H__

#include "stm32f30x_conf.h"
#include "hrtimer.h"
#include "sys_timer.h"
#include "irq.h"
#include "uart.h"
#include "oled.h"
#include "sample.h"
#include "math.h"


#define    INV3P_ON_OFF_FLAG      0x0001
#define    INV3P_CLOSE_FLAG       0x0002
#define    INV3P_PLL_OK_FLAG      0x0004
#define    INV3P_POWER_OK_FLAG    0x0008
#define    INV3P_PLL_CMD_FLAG     0x0010
#define    INV3P_PLL_EN_FLAG      0x0020
#define    INV3P_PWM_A_CLAMP_FLAG 0x0100
#define    INV3P_PWM_B_CLAMP_FLAG 0x0200
#define    INV3P_PWM_C_CLAMP_FLAG 0x0400


#define    IS_INV3P_ON            ((g_inv3pLoop.inv3pStatusFlag & INV3P_ON_OFF_FLAG) == INV3P_ON_OFF_FLAG)
#define    IS_CLOSE_EN            ((g_inv3pLoop.inv3pStatusFlag & INV3P_CLOSE_FLAG) == INV3P_CLOSE_FLAG)
#define    IS_PLL_OK              ((g_inv3pLoop.inv3pStatusFlag & INV3P_PLL_OK_FLAG) == INV3P_PLL_OK_FLAG)
#define    IS_POWER_OK            ((g_inv3pLoop.inv3pStatusFlag & INV3P_POWER_OK_FLAG) == INV3P_POWER_OK_FLAG)

extern float g_iacDReport;
extern float g_iacQReport;
extern float g_vacRmsReport;
extern float g_iacRmsReport;
extern float g_moduReport;
extern float g_iacDReportFilter;
extern float g_iacQReportFilter;
extern float g_vacRmsReportFilter;
extern float g_inv3pIdRef;
extern float g_inv3pIqRef;
extern float g_inv3pIdRefRamp;
extern float g_inv3pIqRefRamp;
extern float g_vdcRealFilter;
extern float g_vacRealAZeroBias;
extern float g_vacRealBZeroBias;
extern float g_vacRealCZeroBias;
extern float g_iacRealAZeroBias;
extern float g_iacRealBZeroBias;
extern float g_iacRealCZeroBias;
extern uint16_t g_pllEN;
extern uint16_t g_powerRunTimeSec;
extern uint16_t g_uartOverTimeCnt;

int16_t AbsInt16(int16_t parameter);
float AbsFloat(float parameter);
void LedBlink(void);
void FanStatusFresh(void);
void SendDataToPc(void);
void ReceiveDataFromPC(void);
void ReportFilter(void);
void RefValueClamp(void);
void Inv3pOnoffProcess(void);
void LowPassFilter(float *output, float *input, float filterTs);
void ParamSoftStart(float *output, float *input, float rampParam);
void Inv3pStatusFresh(void);
void OledFresh(void);

#endif
