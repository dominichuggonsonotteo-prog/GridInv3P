#ifndef __SAMPLE_H__
#define __SAMPLE_H__

#include "stm32f30x_conf.h"
#include "adc.h"

#define    INV3P_VA    g_inv3pVacA.real
#define    INV3P_VB    g_inv3pVacB.real
#define    INV3P_VC    g_inv3pVacC.real
#define    INV3P_IA    g_inv3pIacA.real
#define    INV3P_IB    g_inv3pIacB.real
#define    INV3P_IC    g_inv3pIacC.real
#define    INV3P_VDC   g_inv3pVdc.real

 
typedef struct {
    uint16_t raw;     
    uint16_t rsv;     
    float k;          
    float b;          
    float real;       
} AdcCalcuDef;

extern AdcCalcuDef g_inv3pVacA;
extern AdcCalcuDef g_inv3pVacB;
extern AdcCalcuDef g_inv3pVacC;
extern AdcCalcuDef g_inv3pIacA;
extern AdcCalcuDef g_inv3pIacB;
extern AdcCalcuDef g_inv3pIacC;
extern AdcCalcuDef g_inv3pVdc;

void SampleParamInit(void);
void SampleTransfer(AdcCalcuDef * AdcCalcuParam, uint16_t raw);
void Inv3pSampleCalculate(void);
void Inv3pSampleRaw(void);
void Inv3pAcSampleCorrect(void);

#endif
