/****************************************************************************************
  * @file    dac.c
  * @author  PowerX 功率探索
  * @version V1.0.0
  * @date    01-July-2023
  * @淘宝店铺链接：https://gvmu6dghak5o8m0b3kv7j3j8bx49256.taobao.com/shop/view_shop.htm?spm=a1z0k.7628869.0.0.2a5a37deRiWnIl&shop_id=267446508
  * @LegalDeclaration ：本文档内容难免存在Bug，仅限于交流学习，禁止用于任何的商业用途
  * @Copyright   著作权归 "PowerX 功率探索" 所有
*****************************************************************************************/

#include "dac.h"  // 包含DAC配置的头文件

/**
  * @brief  初始化DAC（数字模拟转换器）
  * @note   配置DAC通道2用于特定应用（如逆变器控制）
  */
void DacInit(void)
{
    DAC_InitTypeDef DAC_InitStructure;  // DAC初始化结构体

    // 使能DAC的APB1总线时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
    
    // 复位DAC1寄存器到默认值
    DAC_DeInit(DAC1);
    
    // 配置DAC初始化参数：
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;  // 不使用触发源（软件触发）
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;  // 不生成波形
    DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bits2_0;  // 未使用（当不使用波形生成时）
    DAC_InitStructure.DAC_Buffer_Switch = DAC_BufferSwitch_Disable;  // 禁用缓冲器切换
    
    // 使用上述配置初始化DAC1的通道2
    DAC_Init(DAC1, DAC_Channel_2, &DAC_InitStructure);
    
    // 使能DAC1的通道2
    DAC_Cmd(DAC1, DAC_Channel_2, ENABLE);
    
    // 设置DAC1通道2的初始数据值（12位右对齐）
    // CBC_INV3P_OF_DAC 是一个预定义的常量，表示逆变器控制应用的初始值
    DAC_SetChannel2Data(DAC1, DAC_Align_12b_R, CBC_INV3P_OF_DAC);
}
