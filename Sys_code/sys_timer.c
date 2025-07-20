/****************************************************************************************
  * @file    sys_timer.c
  * @author  PowerX 功率探索
  * @version V1.0.0
  * @date    01-July-2023
  * @淘宝店铺链接：https://gvmu6dghak5o8m0b3kv7j3j8bx49256.taobao.com/shop/view_shop.htm?spm=a1z0k.7628869.0.0.2a5a37deRiWnIl&shop_id=267446508
  * @LegalDeclaration ：本文档内容难免存在Bug，仅限于交流学习，禁止用于任何的商业用途
  * @Copyright   著作权归 “PowerX 功率探索” 所有
*****************************************************************************************/

#include "sys_timer.h"
#include "nvic.h"

// 全局时间戳ms变量，溢出需要50天，掉电复位
uint32_t g_systemTimeBaseMs = 0;

// TIM6系统时钟初始化函数：用于产生100us慢速中断
void SysTimInit(FunctionPointer IRQHandle)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    
    // 开启TIM6的外设时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
    
    TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;                  // 时基为72分频
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;    // 向上计数模式
    TIM_TimeBaseStructure.TIM_Period = 100 - 1;                    // 周期值为100us
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;        // 不分频
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;               // 每个定时器周期都产生更新事件
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);                // 初始化TIM6

    // 使能TIM6的更新中断
    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
    // 使能TIM6
    TIM_Cmd(TIM6, ENABLE);
    // 100us慢速中断采用最低级优先级
    NvicIrqSet(TIM6_DAC1_IRQn, 15, ENABLE, TIM6_DAC1_IRQHandler);              
}
