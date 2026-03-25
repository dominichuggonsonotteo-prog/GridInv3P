#include "adc.h"

// 全局数组，用于存放ADC采样原始值
uint16_t g_adc1Result[8] = {0};
uint16_t g_adc2Result[8] = {0};

// ADC采样初始化函数
void AdcInit(void)
{
    ADC_InitTypeDef ADC1_InitStructure;
    ADC_InitTypeDef ADC2_InitStructure;
    ADC_CommonInitTypeDef ADC1_CommonInitStructure;
    ADC_CommonInitTypeDef ADC2_CommonInitStructure;
    DMA_InitTypeDef DMA1CH1_InitStructure;
    DMA_InitTypeDef DMA1CH4_InitStructure;

    // 开启ADC与DMA时钟
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ADC12, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 , ENABLE);

    // 设置ADC时钟分频系数
    RCC_ADCCLKConfig(RCC_ADC12PLLCLK_Div1);

    // 复位ADC及其初始化结构体
    ADC_DeInit(ADC1);
    ADC_DeInit(ADC2);
    ADC_StructInit(&ADC1_InitStructure);
    ADC_StructInit(&ADC2_InitStructure);

    // ADC1校准
    ADC_VoltageRegulatorCmd(ADC1, ENABLE); // 使能ADC1电压管理器，用于启动校准
    ADC_SelectCalibrationMode(ADC1, ADC_CalibrationMode_Single); // ADC1的电压校准模式为单端校准
    ADC_StartCalibration(ADC1); // 开始ADC1自校准
    while(ADC_GetCalibrationStatus(ADC1) != RESET ); // 等待ADC1校准完成

    // ADC1工作模式初始化
    ADC1_CommonInitStructure.ADC_Mode = ADC_Mode_Independent; // ADC1独立模式
    ADC1_CommonInitStructure.ADC_Clock = ADC_Clock_AsynClkMode; // ADC1异步时钟模式
    ADC1_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_1; // 选用DMA模式1
    ADC1_CommonInitStructure.ADC_DMAMode = ADC_DMAMode_Circular; // ADC1采用DMA的循环模式
    ADC1_CommonInitStructure.ADC_TwoSamplingDelay = 0; // 采样间隔无相位差
    ADC_CommonInit(ADC1, &ADC1_CommonInitStructure);

    // ADC1采样模式初始化
    ADC1_InitStructure.ADC_ContinuousConvMode = ADC_ContinuousConvMode_Disable; //采用触发模式，即触发事件后进行一轮ADC1通道采样
    ADC1_InitStructure.ADC_Resolution = ADC_Resolution_12b; // ADC1精度12位
    ADC1_InitStructure.ADC_ExternalTrigConvEvent = ADC_ExternalTrigConvEvent_7; //ADC1外部触发源7：对应HRTIM的ADCTRG1事件
    ADC1_InitStructure.ADC_ExternalTrigEventEdge = ADC_ExternalTrigEventEdge_RisingEdge; // ADC1外部触发启动转换：上升沿触发
    ADC1_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; // 采集的数据右对齐
    ADC1_InitStructure.ADC_OverrunMode = ADC_OverrunMode_Disable; // 禁能数据溢出模式
    ADC1_InitStructure.ADC_AutoInjMode = ADC_AutoInjec_Disable; // 禁能自动注入模式
    ADC1_InitStructure.ADC_NbrOfRegChannel = 8; // 扫描采样通道数量8个
    ADC_Init(ADC1, &ADC1_InitStructure);

    // 设置ADC1的采样通道
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_19Cycles5); // 开启通道1， 19.5周期的电压保持时间
    ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 2, ADC_SampleTime_19Cycles5); // 开启通道2， 19.5周期的电压保持时间
    ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 3, ADC_SampleTime_19Cycles5); // 开启通道3， 19.5周期的电压保持时间
    ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 4, ADC_SampleTime_19Cycles5); // 开启通道4， 19.5周期的电压保持时间
    ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 5, ADC_SampleTime_19Cycles5); // 开启通道7， 19.5周期的电压保持时间
    ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 6, ADC_SampleTime_19Cycles5); // 开启通道8， 19.5周期的电压保持时间
    ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 7, ADC_SampleTime_19Cycles5); // 开启通道11， 19.5周期的电压保持时间
    ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 8, ADC_SampleTime_19Cycles5); // 开启通道12， 19.5周期的电压保持时间

    // 使能ADC1
    ADC_Cmd(ADC1, ENABLE); // ADC1使能
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_RDY)); // 等待ADC1就绪
    ADC_DMACmd(ADC1, ENABLE); // 使能ADC1的DMA功能
    ADC_DMAConfig(ADC1, ADC_DMAMode_Circular); // ADC1的DMA为循环模式
    ADC_StartConversion(ADC1); // 采集开始

    // ADC2校准
    ADC_VoltageRegulatorCmd(ADC2, ENABLE); // 使能ADC2电压管理器，用于启动校准
    ADC_SelectCalibrationMode(ADC2, ADC_CalibrationMode_Single); // ADC2的电压校准模式为单端校准
    ADC_StartCalibration(ADC2); // 开始ADC2自校准
    while(ADC_GetCalibrationStatus(ADC2) != RESET ); // 等待ADC2校准完成

    // ADC2工作模式初始化
    ADC2_CommonInitStructure.ADC_Mode = ADC_Mode_Independent; //ADC2独立模式
    ADC2_CommonInitStructure.ADC_Clock = ADC_Clock_AsynClkMode; // ADC2异步时钟模式
    ADC2_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_1; // 选用DMA模式1
    ADC2_CommonInitStructure.ADC_DMAMode = ADC_DMAMode_Circular; // ADC2采用DMA的循环模式
    ADC2_CommonInitStructure.ADC_TwoSamplingDelay = 0; // 采样间隔无相位差
    ADC_CommonInit(ADC2, &ADC2_CommonInitStructure);

    // ADC2采样模式初始化
    ADC2_InitStructure.ADC_ContinuousConvMode = ADC_ContinuousConvMode_Disable; // 采用触发模式
    ADC2_InitStructure.ADC_Resolution = ADC_Resolution_12b; // ADC2精度12位
    ADC2_InitStructure.ADC_ExternalTrigConvEvent = ADC_ExternalTrigConvEvent_7; //ADC2外部触发源7：对应HRTIM的ADCTRG1事件
    ADC2_InitStructure.ADC_ExternalTrigEventEdge = ADC_ExternalTrigEventEdge_RisingEdge; // ADC2外部触发启动转换：上升沿触发
    ADC2_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; // 采集的数据右对齐
    ADC2_InitStructure.ADC_OverrunMode = ADC_OverrunMode_Disable; // 禁能数据溢出模式
    ADC2_InitStructure.ADC_AutoInjMode = ADC_AutoInjec_Disable; // 禁能自动注入模式
    ADC2_InitStructure.ADC_NbrOfRegChannel = 8;
    ADC_Init(ADC2, &ADC2_InitStructure);

    // 设置ADC2的采样通道
    ADC_RegularChannelConfig(ADC2, ADC_Channel_1, 1, ADC_SampleTime_19Cycles5); // 开启通道1， 19.5周期的电压保持时间
    ADC_RegularChannelConfig(ADC2, ADC_Channel_2, 2, ADC_SampleTime_19Cycles5); // 开启通道2， 19.5周期的电压保持时间
    ADC_RegularChannelConfig(ADC2, ADC_Channel_3, 3, ADC_SampleTime_19Cycles5); // 开启通道3， 19.5周期的电压保持时间
    ADC_RegularChannelConfig(ADC2, ADC_Channel_4, 4, ADC_SampleTime_19Cycles5); // 开启通道4， 19.5周期的电压保持时间
    ADC_RegularChannelConfig(ADC2, ADC_Channel_5, 5, ADC_SampleTime_19Cycles5); // 开启通道5， 19.5周期的电压保持时间
    ADC_RegularChannelConfig(ADC2, ADC_Channel_6, 6, ADC_SampleTime_19Cycles5); // 开启通道6， 19.5周期的电压保持时间
    ADC_RegularChannelConfig(ADC2, ADC_Channel_11, 7, ADC_SampleTime_19Cycles5); // 开启通道11， 19.5周期的电压保持时间
    ADC_RegularChannelConfig(ADC2, ADC_Channel_12, 8, ADC_SampleTime_19Cycles5); // 开启通道12， 19.5周期的电压保持时间

    // 使能ADC2
    ADC_Cmd(ADC2, ENABLE); // ADC2使能
    while(!ADC_GetFlagStatus(ADC2, ADC_FLAG_RDY)); // 等待ADC2就绪
    ADC_DMACmd(ADC2, ENABLE); // 使能ADC2的DMA功能
    ADC_DMAConfig(ADC2, ADC_DMAMode_Circular); // ADC2的DMA为循环模式
    ADC_StartConversion(ADC2); // 采集开始

    //ADC关联的DMA通道初始化
    DMA_DeInit(DMA1_Channel1); // DMA1通道1复位到默认值
    DMA_DeInit(DMA1_Channel4); // DMA1通道4复位到默认值
    SYSCFG_DMAChannelRemapConfig(SYSCFG_DMARemapCh4_ADC2, ENABLE); // DMA1通道4重映射到ADC2

    // DMA1通道1初始化
    DMA1CH1_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (ADC1->DR); //DMA1通道1外设端连接到ADC1的数据寄存器
    DMA1CH1_InitStructure.DMA_MemoryBaseAddr = (uint32_t) g_adc1Result; //DMA1通道1内存端连接到g_adc1Result数组的起始地址
    DMA1CH1_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; // 从外设读数据
    DMA1CH1_InitStructure.DMA_BufferSize = 8; // 读取数据长度为8字节
    DMA1CH1_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // 外设地址不递增
    DMA1CH1_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; // 内存地址递增
    DMA1CH1_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; // 一次读取半字16bit
    DMA1CH1_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; // 一次存储半字16bit
    DMA1CH1_InitStructure.DMA_Mode = DMA_Mode_Circular; // DMA为循环模式
    DMA1CH1_InitStructure.DMA_Priority = DMA_Priority_High; // 优先级设定为高
    DMA1CH1_InitStructure.DMA_M2M = DMA_M2M_Disable; // DMA内存互访关闭
    DMA_Init(DMA1_Channel1, &DMA1CH1_InitStructure);

    // DMA1通道4初始化
    DMA1CH4_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (ADC2->DR); //DMA1通道4外设端连接到ADC2的数据寄存器
    DMA1CH4_InitStructure.DMA_MemoryBaseAddr = (uint32_t) g_adc2Result; //DMA1通道4内存端连接到g_adc2Result数组的起始地址
    DMA1CH4_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; // 从外设读数据
    DMA1CH4_InitStructure.DMA_BufferSize = 8; // 读取数据长度为8字节
    DMA1CH4_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // 外设地址不递增
    DMA1CH4_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; // 内存地址递增
    DMA1CH4_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; // 一次读取半字16bit
    DMA1CH4_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; // 一次存储半字16bit
    DMA1CH4_InitStructure.DMA_Mode = DMA_Mode_Circular; // DMA为循环模式
    DMA1CH4_InitStructure.DMA_Priority = DMA_Priority_VeryHigh; // 优先级设定为高
    DMA1CH4_InitStructure.DMA_M2M = DMA_M2M_Disable; // DMA内存互访关闭
    DMA_Init(DMA1_Channel4, &DMA1CH4_InitStructure);

    // 使能DMA1通道1和通道4
    DMA_Cmd(DMA1_Channel1, ENABLE);
    DMA_Cmd(DMA1_Channel4, ENABLE);

    // 开启DMA1通道1传输结束中断
    DMA_ITConfig(DMA1_Channel1,DMA_IT_TC,ENABLE);
}