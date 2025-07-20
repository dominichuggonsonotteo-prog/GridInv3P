/****************************************************************************************
  * @file    adc.c
  * @author  PowerX 功率探索
  * @version V1.0.0
  * @date    01-July-2023
  * @淘宝店铺链接：https://gvmu6dghak5o8m0b3kv7j3j8bx49256.taobao.com/shop/view_shop.htm?spm=a1z0k.7628869.0.0.2a5a37deRiWnIl&shop_id=267446508
  * @LegalDeclaration ：本文档内容难免存在Bug，仅限于交流学习，禁止用于任何的商业用途
  * @Copyright   著作权归 "PowerX 功率探索" 所有
*****************************************************************************************/

#include "adc.h"  // 包含ADC配置的头文件

// ADC1转换结果数组，用于存储8个通道的转换值
uint16_t g_adc1Result[8] = {0};

// ADC2转换结果数组，用于存储8个通道的转换值
uint16_t g_adc2Result[8] = {0};

/**
  * @brief  初始化ADC和DMA配置
  * @note   配置两个ADC模块（ADC1和ADC2）以及DMA传输，用于逆变器控制的电压和电流采样
  */
void AdcInit(void)
{
    // ADC初始化结构体
    ADC_InitTypeDef ADC1_InitStructure;
    ADC_InitTypeDef ADC2_InitStructure;
    
    // ADC通用初始化结构体
    ADC_CommonInitTypeDef ADC1_CommonInitStructure;
    ADC_CommonInitTypeDef ADC2_CommonInitStructure;
    
    // DMA初始化结构体
    DMA_InitTypeDef DMA1CH1_InitStructure;
    DMA_InitTypeDef DMA1CH4_InitStructure;
    
    // 使能ADC12和DMA1的AHB总线时钟
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ADC12, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 , ENABLE);
    
    // 配置ADC时钟源为PLL（不分频）
    RCC_ADCCLKConfig(RCC_ADC12PLLCLK_Div1);
    
    // 复位ADC1和ADC2寄存器到默认值
    ADC_DeInit(ADC1);
    ADC_DeInit(ADC2);
    
    // 初始化ADC结构体为默认值
    ADC_StructInit(&ADC1_InitStructure);
    ADC_StructInit(&ADC2_InitStructure);

    /************************ ADC1 配置 ************************/
    // 使能ADC1的电压调节器
    ADC_VoltageRegulatorCmd(ADC1, ENABLE);
    
    // 选择单次校准模式
    ADC_SelectCalibrationMode(ADC1, ADC_CalibrationMode_Single);
    
    // 开始ADC1校准
    ADC_StartCalibration(ADC1);
    
    // 等待校准完成
    while(ADC_GetCalibrationStatus(ADC1) != RESET);
    
    // 配置ADC1通用参数：
    ADC1_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;              // 独立模式
    ADC1_CommonInitStructure.ADC_Clock = ADC_Clock_AsynClkMode;            // 异步时钟模式
    ADC1_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_1;      // DMA访问模式1
    ADC1_CommonInitStructure.ADC_DMAMode = ADC_DMAMode_Circular;           // 循环DMA模式
    ADC1_CommonInitStructure.ADC_TwoSamplingDelay = 0;                     // 双采样延迟为0
    ADC_CommonInit(ADC1, &ADC1_CommonInitStructure);
    
    // 配置ADC1特定参数：
    ADC1_InitStructure.ADC_ContinuousConvMode = ADC_ContinuousConvMode_Disable;          // 禁用连续转换模式
    ADC1_InitStructure.ADC_Resolution = ADC_Resolution_12b;                              // 12位分辨率
    ADC1_InitStructure.ADC_ExternalTrigConvEvent = ADC_ExternalTrigConvEvent_7;          // 外部触发事件7
    ADC1_InitStructure.ADC_ExternalTrigEventEdge = ADC_ExternalTrigEventEdge_RisingEdge; // 上升沿触发
    ADC1_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;                              // 数据右对齐
    ADC1_InitStructure.ADC_OverrunMode = ADC_OverrunMode_Disable;                        // 禁用溢出模式
    ADC1_InitStructure.ADC_AutoInjMode = ADC_AutoInjec_Disable;                          // 禁用自动注入模式
    ADC1_InitStructure.ADC_NbrOfRegChannel = 8;                                          // 8个规则通道
    ADC_Init(ADC1, &ADC1_InitStructure);
    
    // 配置ADC1规则通道（采样时间和顺序）：
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_19Cycles5);  // 通道1，顺序1，采样时间19.5周期
    ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 2, ADC_SampleTime_19Cycles5);  // 通道2，顺序2，采样时间19.5周期
    ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 3, ADC_SampleTime_19Cycles5);  // 通道3，顺序3，采样时间19.5周期
    ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 4, ADC_SampleTime_19Cycles5);  // 通道4，顺序4，采样时间19.5周期
    ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 5, ADC_SampleTime_19Cycles5);  // 通道7，顺序5，采样时间19.5周期
    ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 6, ADC_SampleTime_19Cycles5);  // 通道8，顺序6，采样时间19.5周期
    ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 7, ADC_SampleTime_19Cycles5); // 通道11，顺序7，采样时间19.5周期
    ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 8, ADC_SampleTime_19Cycles5); // 通道12，顺序8，采样时间19.5周期
    
    // 使能ADC1
    ADC_Cmd(ADC1, ENABLE);
    
    // 等待ADC1准备就绪
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_RDY));
    
    // 使能ADC1的DMA传输
    ADC_DMACmd(ADC1, ENABLE);
    
    // 配置ADC1为循环DMA模式
    ADC_DMAConfig(ADC1, ADC_DMAMode_Circular);
    
    // 启动ADC1转换
    ADC_StartConversion(ADC1);

    /************************ ADC2 配置 ************************/
    // 使能ADC2的电压调节器
    ADC_VoltageRegulatorCmd(ADC2, ENABLE);
    
    // 选择单次校准模式
    ADC_SelectCalibrationMode(ADC2, ADC_CalibrationMode_Single);
    
    // 开始ADC2校准
    ADC_StartCalibration(ADC2);
    
    // 等待校准完成
    while(ADC_GetCalibrationStatus(ADC2) != RESET);
    
    // 配置ADC2通用参数：
    ADC2_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;              // 独立模式
    ADC2_CommonInitStructure.ADC_Clock = ADC_Clock_AsynClkMode;            // 异步时钟模式
    ADC2_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_1;      // DMA访问模式1
    ADC2_CommonInitStructure.ADC_DMAMode = ADC_DMAMode_Circular;           // 循环DMA模式
    ADC2_CommonInitStructure.ADC_TwoSamplingDelay = 0;                     // 双采样延迟为0
    ADC_CommonInit(ADC2, &ADC2_CommonInitStructure);
    
    // 配置ADC2特定参数：
    ADC2_InitStructure.ADC_ContinuousConvMode = ADC_ContinuousConvMode_Disable;          // 禁用连续转换模式
    ADC2_InitStructure.ADC_Resolution = ADC_Resolution_12b;                              // 12位分辨率
    ADC2_InitStructure.ADC_ExternalTrigConvEvent = ADC_ExternalTrigConvEvent_7;          // 外部触发事件7
    ADC2_InitStructure.ADC_ExternalTrigEventEdge = ADC_ExternalTrigEventEdge_RisingEdge; // 上升沿触发
    ADC2_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;                              // 数据右对齐
    ADC2_InitStructure.ADC_OverrunMode = ADC_OverrunMode_Disable;                        // 禁用溢出模式
    ADC2_InitStructure.ADC_AutoInjMode = ADC_AutoInjec_Disable;                          // 禁用自动注入模式
    ADC2_InitStructure.ADC_NbrOfRegChannel = 8;                                          // 8个规则通道
    ADC_Init(ADC2, &ADC2_InitStructure);
    
    // 配置ADC2规则通道（采样时间和顺序）：
    ADC_RegularChannelConfig(ADC2, ADC_Channel_1, 1, ADC_SampleTime_19Cycles5);  // 通道1，顺序1，采样时间19.5周期
    ADC_RegularChannelConfig(ADC2, ADC_Channel_2, 2, ADC_SampleTime_19Cycles5);  // 通道2，顺序2，采样时间19.5周期
    ADC_RegularChannelConfig(ADC2, ADC_Channel_3, 3, ADC_SampleTime_19Cycles5);  // 通道3，顺序3，采样时间19.5周期
    ADC_RegularChannelConfig(ADC2, ADC_Channel_4, 4, ADC_SampleTime_19Cycles5);  // 通道4，顺序4，采样时间19.5周期
    ADC_RegularChannelConfig(ADC2, ADC_Channel_5, 5, ADC_SampleTime_19Cycles5);  // 通道5，顺序5，采样时间19.5周期
    ADC_RegularChannelConfig(ADC2, ADC_Channel_6, 6, ADC_SampleTime_19Cycles5);  // 通道6，顺序6，采样时间19.5周期
    ADC_RegularChannelConfig(ADC2, ADC_Channel_11, 7, ADC_SampleTime_19Cycles5); // 通道11，顺序7，采样时间19.5周期
    ADC_RegularChannelConfig(ADC2, ADC_Channel_12, 8, ADC_SampleTime_19Cycles5); // 通道12，顺序8，采样时间19.5周期
    
    // 使能ADC2
    ADC_Cmd(ADC2, ENABLE);
    
    // 等待ADC2准备就绪
    while(!ADC_GetFlagStatus(ADC2, ADC_FLAG_RDY));
    
    // 使能ADC2的DMA传输
    ADC_DMACmd(ADC2, ENABLE);
    
    // 配置ADC2为循环DMA模式
    ADC_DMAConfig(ADC2, ADC_DMAMode_Circular);
    
    // 启动ADC2转换
    ADC_StartConversion(ADC2);

    /************************ DMA 配置 ************************/
    // 复位DMA通道1和4
    DMA_DeInit(DMA1_Channel1);
    DMA_DeInit(DMA1_Channel4);
    
    // 重新映射DMA通道4到ADC2
    SYSCFG_DMAChannelRemapConfig(SYSCFG_DMARemapCh4_ADC2, ENABLE);
    
    // 配置DMA通道1（用于ADC1）：
    DMA1CH1_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (ADC1->DR);                 // 外设地址：ADC1数据寄存器
    DMA1CH1_InitStructure.DMA_MemoryBaseAddr = (uint32_t) g_adc1Result;                     // 内存地址：g_adc1Result数组
    DMA1CH1_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                                  // 传输方向：外设到内存
    DMA1CH1_InitStructure.DMA_BufferSize = 8;                                               // 缓冲区大小：8个元素
    DMA1CH1_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;                    // 外设地址不递增
    DMA1CH1_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                             // 内存地址递增
    DMA1CH1_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;         // 外设数据大小：半字(16位)
    DMA1CH1_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;                 // 内存数据大小：半字(16位)
    DMA1CH1_InitStructure.DMA_Mode = DMA_Mode_Circular;                                     // 循环模式
    DMA1CH1_InitStructure.DMA_Priority = DMA_Priority_High;                                 // 高优先级
    DMA1CH1_InitStructure.DMA_M2M = DMA_M2M_Disable;                                        // 禁用内存到内存模式
    DMA_Init(DMA1_Channel1, &DMA1CH1_InitStructure);
    
    // 配置DMA通道4（用于ADC2）：
    DMA1CH4_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (ADC2->DR);                 // 外设地址：ADC2数据寄存器
    DMA1CH4_InitStructure.DMA_MemoryBaseAddr = (uint32_t) g_adc2Result;                     // 内存地址：g_adc2Result数组
    DMA1CH4_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                                  // 传输方向：外设到内存
    DMA1CH4_InitStructure.DMA_BufferSize = 8;                                               // 缓冲区大小：8个元素
    DMA1CH4_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;                    // 外设地址不递增
    DMA1CH4_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                             // 内存地址递增
    DMA1CH4_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;         // 外设数据大小：半字(16位)
    DMA1CH4_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;                 // 内存数据大小：半字(16位)
    DMA1CH4_InitStructure.DMA_Mode = DMA_Mode_Circular;                                     // 循环模式
    DMA1CH4_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                             // 非常高优先级
    DMA1CH4_InitStructure.DMA_M2M = DMA_M2M_Disable;                                        // 禁用内存到内存模式
    DMA_Init(DMA1_Channel4, &DMA1CH4_InitStructure);
    
    // 使能DMA通道1和4
    DMA_Cmd(DMA1_Channel1, ENABLE);
    DMA_Cmd(DMA1_Channel4, ENABLE);
    
    // 使能DMA通道1的传输完成中断
    DMA_ITConfig(DMA1_Channel1,DMA_IT_TC,ENABLE);
}
