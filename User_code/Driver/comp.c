/****************************************************************************************
  * @file    comp.c
  * @author  PowerX 功率探索
  * @version V1.0.0
  * @date    01-July-2023
  * @淘宝店铺链接：https://gvmu6dghak5o8m0b3kv7j3j8bx49256.taobao.com/shop/view_shop.htm?spm=a1z0k.7628869.0.0.2a5a37deRiWnIl&shop_id=267446508
  * @LegalDeclaration ：本文档内容难免存在Bug，仅限于交流学习，禁止用于任何的商业用途
  * @Copyright   著作权归 "PowerX 功率探索" 所有
*****************************************************************************************/

#include "comp.h"  // 包含比较器配置的头文件

/**
  * @brief  初始化比较器COMP4
  * @note   配置比较器的输入源、输出目标和其他参数，用于逆变器控制
  */
void CompInit(void)
{
    COMP_InitTypeDef COMP_InitStructure;  // 比较器初始化结构体

    // 配置比较器初始化参数：
    COMP_InitStructure.COMP_InvertingInput = COMP_InvertingInput_DAC1OUT2;   // 反相输入源选择DAC1通道2的输出
    COMP_InitStructure.COMP_NonInvertingInput = COMP_NonInvertingInput_IO1;  // 同相输入源选择IO1（具体引脚需要参考硬件设计）
    COMP_InitStructure.COMP_Output = COMP_Output_HRTIM1_EE2_2;               // 输出路由到HRTIM1的EE2_2（用于PWM控制）
    COMP_InitStructure.COMP_BlankingSrce = COMP_BlankingSrce_None;           // 不使用消隐源（无消隐时间）
    COMP_InitStructure.COMP_OutputPol = COMP_OutputPol_NonInverted;          // 输出极性为非反转（同相输入>反相输入时输出高）
    COMP_InitStructure.COMP_Hysteresis = COMP_Hysteresis_No;                 // 无迟滞（高速响应）
    COMP_InitStructure.COMP_Mode = COMP_Mode_HighSpeed;                      // 高速模式（快速响应输入变化）
    
    // 使用上述配置初始化COMP4比较器
    COMP_Init(COMP_Selection_COMP4, &COMP_InitStructure);
    
    // 使能COMP4比较器
    COMP_Cmd(COMP_Selection_COMP4, ENABLE);    
}
