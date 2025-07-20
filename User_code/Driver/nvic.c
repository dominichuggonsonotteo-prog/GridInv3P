/****************************************************************************************
  * @file    nvic.c
  * @author  PowerX 功率探索
  * @version V1.0.0
  * @date    01-July-2023
  * @淘宝店铺链接：https://gvmu6dghak5o8m0b3kv7j3j8bx49256.taobao.com/shop/view_shop.htm?spm=a1z0k.7628869.0.0.2a5a37deRiWnIl&shop_id=267446508
  * @LegalDeclaration ：本文档内容难免存在Bug，仅限于交流学习，禁止用于任何的商业用途
  * @Copyright   著作权归 "PowerX 功率探索" 所有
*****************************************************************************************/

#include "nvic.h"  // 包含NVIC配置的头文件

/**
  * @brief  配置NVIC中断设置
  * @param  channel: 中断通道号
  * @param  priority: 抢占优先级（0-15）
  * @param  status: 中断使能状态（ENABLE或DISABLE）
  * @param  IRQHandle: 中断服务函数指针
  * @note   此函数用于配置嵌套向量中断控制器(NVIC)，设置中断优先级和使能状态。
  *         使用优先级分组4（4位抢占优先级，0位子优先级）。
  *         注意：中断服务函数的实际注册在启动文件中完成，这里仅配置NVIC参数。
  */
void NvicIrqSet(uint8_t channel, uint8_t priority, FunctionalState status, FunctionPointer IRQHandle) {
    // 检查中断服务函数指针是否有效
    if (IRQHandle == NULL) {
        return;  // 无效指针，直接返回
    }

    // 定义NVIC初始化结构体
    NVIC_InitTypeDef NVIC_InitStructure;

    // 设置NVIC优先级分组为4（4位抢占优先级，0位子优先级）
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    // 配置NVIC初始化参数：
    NVIC_InitStructure.NVIC_IRQChannel = channel;                     // 设置中断通道
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;                // 子优先级固定为0（分组4不使用子优先级）
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = priority;  // 设置抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = status;                   // 设置中断使能状态
    
    // 应用NVIC配置
    NVIC_Init(&NVIC_InitStructure);
}
