#ifndef __ADC_H__
#define __ADC_H__

#include "stm32f30x_conf.h"
#include "sys_timer.h"

extern uint16_t g_adc1Result[8];
extern uint16_t g_adc2Result[8];

#define IAC1_ADC_RAW  g_adc1Result[0]
#define IAC2_ADC_RAW  g_adc1Result[1]
#define IAC3_ADC_RAW  g_adc1Result[2]
#define VDC1_ADC_RAW  g_adc1Result[3]
#define VAC2_ADC_RAW  g_adc1Result[4]
#define VAC3_ADC_RAW  g_adc1Result[5]
#define ICBC2_ADC_RAW g_adc1Result[6]
#define IDC2_ADC_RAW  g_adc1Result[7]

#define IDC1_ADC_RAW  g_adc2Result[0]
#define TEMP1_ADC_RAW g_adc2Result[1]
#define TEMP2_ADC_RAW g_adc2Result[2]
#define ICBC1_ADC_RAW g_adc2Result[3]
#define IREF_ADC_RAW  g_adc2Result[4]
#define VAC1_ADC_RAW  g_adc2Result[5]
#define VREF_ADC_RAW  g_adc2Result[6]
#define VDC2_ADC_RAW  g_adc2Result[7]

void AdcInit(void);

#endif
