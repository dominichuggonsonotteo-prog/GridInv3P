/****************************************************************************************
  * @file    timer.c
  * @author  PowerX 功率探索
  * @version V1.0.0
  * @date    01-July-2023
  * @淘宝店铺链接：https://gvmu6dghak5o8m0b3kv7j3j8bx49256.taobao.com/shop/view_shop.htm?spm=a1z0k.7628869.0.0.2a5a37deRiWnIl&shop_id=267446508
  * @LegalDeclaration ：本文档内容难免存在Bug，仅限于交流学习，禁止用于任何的商业用途
  * @Copyright   著作权归 "PowerX 功率探索" 所有
*****************************************************************************************/

#include "timer.h"  // 包含定时器配置的头文件
#include "gpio.h"   // 包含GPIO配置的头文件（可能用于定时器引脚配置）

/**
  * @brief  初始化定时器TIM2和TIM7
  * @note   TIM2配置为PWM输出模式（通道3），用于控制逆变器的驱动信号
  *         TIM7配置为基础定时器，用于提供精确延时
  */
void TimInit (void)
{
    // 定义定时器时基初始化结构和输出比较初始化结构
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure; 
    TIM_OCInitTypeDef TIM_OCInitStructure;

    /************************ TIM2 配置（PWM输出） ************************/
    // 使能TIM2的APB1总线时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
     
    // 配置TIM2时基参数：
    TIM_TimeBaseStructure.TIM_Prescaler = 0;                                    // 预分频器为0（不分频）
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;                 // 向上计数模式
    TIM_TimeBaseStructure.TIM_Period = 72000 - 1;                               // 自动重装载值（周期）
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;                     // 时钟分频因子为1
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;                            // 重复计数器为0
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);                             // 初始化TIM2
     
    // 配置TIM2输出比较参数（通道3）：
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;                           // PWM模式2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;               // 使能输出
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;            // 禁用互补输出
    TIM_OCInitStructure.TIM_Pulse = 9000 - 1;                                   // 设置占空比的脉冲值（比较值）
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;                   // 输出极性高
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCPolarity_High;                  // 互补输出极性高
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;                // 空闲状态为复位
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;               // 互补输出空闲状态为复位
    TIM_OC3Init(TIM2, &TIM_OCInitStructure);                                    // 初始化TIM2通道3
     
    // 使能TIM2的主输出（用于高级定时器，但TIM2是通用定时器，此函数可能无效果，但保留以兼容）
    TIM_CtrlPWMOutputs(TIM2, ENABLE);
     
    // 暂时禁用TIM2（稍后可能在应用层使能）
    TIM_Cmd(TIM2, DISABLE);

    /************************ TIM7 配置（基础定时器） ************************/
    // 使能TIM7的APB1总线时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
     
    // 配置TIM7时基参数：
    TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;                               // 预分频器71（72分频，得到1MHz时钟）
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;                 // 向上计数模式
    TIM_TimeBaseStructure.TIM_Period = 0xFFFF;                                  // 自动重装载值（最大65535）
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;                     // 时钟分频因子为1
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;                            // 重复计数器为0
    TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);                             // 初始化TIM7
     
    // 使能TIM7
    TIM_Cmd(TIM7, ENABLE);
}

/**
  * @brief  毫秒级延时函数
  * @param  nCount: 延时的毫秒数
  * @note   通过循环调用微秒级延时实现
  */
void DelayMs(uint16_t nCount)
{
    while (nCount != 0) {
        nCount--;
        DelayUs(1000);  // 延时1000微秒（1毫秒）
    }
}

/**
  * @brief  微秒级延时函数
  * @param  time: 延时的微秒数
  * @note   使用TIM7进行精确延时，TIM7时钟为1MHz（1计数=1微秒）
  */
void DelayUs(uint16_t time)
{
    TIM7->CNT = 0;            // 重置TIM7计数器
    while((TIM7->CNT) < time); // 等待计数器达到指定值
}
