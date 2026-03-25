
#include "sample.h"  // 包含采样相关的头文件

// 三相电压电流和直流电压的ADC计算结构体
AdcCalcuDef g_inv3pVacA = {0};  // A相交流电压采样参数
AdcCalcuDef g_inv3pVacB = {0};  // B相交流电压采样参数
AdcCalcuDef g_inv3pVacC = {0};  // C相交流电压采样参数
AdcCalcuDef g_inv3pIacA = {0};  // A相交流电流采样参数
AdcCalcuDef g_inv3pIacB = {0};  // B相交流电流采样参数
AdcCalcuDef g_inv3pIacC = {0};  // C相交流电流采样参数
AdcCalcuDef g_inv3pVdc = {0};   // 直流电压采样参数

// ADC原始采样值变量
uint16_t g_inv3pVacARaw = 0;  // A相交流电压原始ADC值
uint16_t g_inv3pVacBRaw = 0;  // B相交流电压原始ADC值
uint16_t g_inv3pVacCRaw = 0;  // C相交流电压原始ADC值
uint16_t g_inv3pIacARaw = 0;  // A相交流电流原始ADC值
uint16_t g_inv3pIacBRaw = 0;  // B相交流电流原始ADC值
uint16_t g_inv3pIacCRaw = 0;  // C相交流电流原始ADC值
uint16_t g_inv3pVdcRaw = 0;   // 直流电压原始ADC值

// 采样折算系数配置函数
void static inline SampleParamConfig(AdcCalcuDef * AdcCalcuParam, float k, float b)
{
    AdcCalcuParam->k = k;  // 设置斜率参数
    AdcCalcuParam->b = b;  // 设置偏移量参数
}

// 根据ADC采样原始值计算实际值函数
CCMRAM void SampleTransfer(AdcCalcuDef * AdcCalcuParam, uint16_t raw)
{
    AdcCalcuParam->raw = raw;  // 保存原始ADC值
    // 计算实际物理量值：实际值 = 原始值 * k + b
    AdcCalcuParam->real = AdcCalcuParam->raw * AdcCalcuParam->k + AdcCalcuParam->b;
}

// 采样折算系数初始化函数
void SampleParamInit(void)
{
    // 配置三相交流电压采样参数（斜率0.04884，偏移-99.737）
    SampleParamConfig(&g_inv3pVacA, 0.04884f, -99.737f);
    SampleParamConfig(&g_inv3pVacB, 0.04884f, -99.737f);
    SampleParamConfig(&g_inv3pVacC, 0.04884f, -99.737f);
    
    // 配置三相交流电流采样参数（斜率-0.013472，偏移25.5）
    SampleParamConfig(&g_inv3pIacA, -0.013472f, 25.5f);
    SampleParamConfig(&g_inv3pIacB, -0.013472f, 25.5f);
    SampleParamConfig(&g_inv3pIacC, -0.013472f, 25.5f);
    
    // 配置直流电压采样参数（斜率0.04884，偏移0.0）
    SampleParamConfig(&g_inv3pVdc, 0.04884f, 0.0f);
}

// 双采样打拍函数
void Inv3pSampleRaw(void)
{
    g_inv3pVacARaw = VAC3_ADC_RAW;  // 读取A相交流电压原始ADC值
    g_inv3pVacBRaw = VAC2_ADC_RAW;  // 读取B相交流电压原始ADC值
    g_inv3pVacCRaw = VAC1_ADC_RAW;  // 读取C相交流电压原始ADC值
    g_inv3pIacARaw = IAC1_ADC_RAW;  // 读取A相交流电流原始ADC值
    g_inv3pIacBRaw = IAC2_ADC_RAW;  // 读取B相交流电流原始ADC值
    g_inv3pIacCRaw = IAC3_ADC_RAW;  // 读取C相交流电流原始ADC值
    g_inv3pVdcRaw = VDC1_ADC_RAW;   // 读取直流电压原始ADC值
}

// 实际采样值计算函数
void Inv3pSampleCalculate(void)
{
    // 对三相交流电压进行滑动平均并转换为实际值
    SampleTransfer(&g_inv3pVacA, (VAC3_ADC_RAW + g_inv3pVacARaw) >> 1);  // A相电压
    SampleTransfer(&g_inv3pVacB, (VAC2_ADC_RAW + g_inv3pVacBRaw) >> 1);  // B相电压
    SampleTransfer(&g_inv3pVacC, (VAC1_ADC_RAW + g_inv3pVacCRaw) >> 1);  // C相电压
    
    // 对三相交流电流进行滑动平均并转换为实际值
    SampleTransfer(&g_inv3pIacA, (IAC1_ADC_RAW + g_inv3pIacARaw) >> 1);  // A相电流
    SampleTransfer(&g_inv3pIacB, (IAC2_ADC_RAW + g_inv3pIacBRaw) >> 1);  // B相电流
    SampleTransfer(&g_inv3pIacC, (IAC3_ADC_RAW + g_inv3pIacCRaw) >> 1);  // C相电流
    
    // 对直流电压进行滑动平均并转换为实际值
    SampleTransfer(&g_inv3pVdc, (VDC1_ADC_RAW + g_inv3pVdcRaw) >> 1);    // 直流电压
}

// 交流采样值的零偏校准
void Inv3pAcSampleCorrect(void)
{
    // 校正三相交流电压（减去零点偏移）
    g_vacRealACorrect = INV3P_VA - g_vacRealAZeroBias;  // A相电压校正
    g_vacRealBCorrect = INV3P_VB - g_vacRealBZeroBias;  // B相电压校正
    g_vacRealCCorrect = INV3P_VC - g_vacRealCZeroBias;  // C相电压校正
    
    // 校正三相交流电流（减去零点偏移）
    g_iacRealACorrect = INV3P_IA - g_iacRealAZeroBias;  // A相电流校正
    g_iacRealBCorrect = INV3P_IB - g_iacRealBZeroBias;  // B相电流校正
    g_iacRealCCorrect = INV3P_IC - g_iacRealCZeroBias;  // C相电流校正
}
