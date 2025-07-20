#ifndef __PROTECT_H__
#define __PROTECT_H__

#include "stm32f30x_conf.h"
#include "inv3p.h"
#include "timer.h"
#include "gpio.h"

#define    CNT_20MS_100US          200
#define    CNT_30MS_100US          300
#define    CNT_3S_10MS             300
#define    CNT_100MS_100US         1000

#define    INV3P_FAST_VDC_OVP      0x0001         
#define    INV3P_SLOW_VDC_OVP      0x0002         
#define    INV3P_FAST_IDC_OCP      0x0004         
#define    INV3P_SLOW_IDC_OCP      0x0008         
#define    INV3P_FAST_AC_OVP       0x0010         
#define    INV3P_SLOW_AC_OVP       0x0020         
#define    INV3P_FAST_AC_OCP       0x0040         
#define    INV3P_SLOW_AC_OCP       0x0080         
#define    INV3P_UART_OVER_TIME    0x0100         
#define    INV3P_ADC_GATE_ERROR    0x0200         

#define    DC_FAST_OVP_THRESHOLD   (75.0f)        
#define    DC_SLOW_OVP_THRESHOLD   (73.0f)        

#define    AC_FAST_OVP_THRESHOLD   ((30.0f) * (1.414f))   
#define    AC_FAST_OCP_THRESHOLD   ((10.0f) * (1.414f))   
#define    AC_SLOW_OVP_THRESHOLD   (25.0f)                
#define    AC_SLOW_OCP_THRESHOLD   (7.5f)                 

void ProtectPoll(void);
void Inv3pProtectClear(void);
void FastProtectDetect(float * protectParamValue, float protectValue, uint16_t protectBit);
void SlowProtectDetect(float * protectParamValue, float protectValue, uint16_t overTime, uint16_t protectBit, uint16_t * overTimeCnt);

#endif

