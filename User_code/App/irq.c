/****************************************************************************************
  * @file    irq.c
  * @author  PowerX 功率探索
  * @version V1.0.0
  * @date    01-July-2023
  * @淘宝店铺链接：https://gvmu6dghak5o8m0b3kv7j3j8bx49256.taobao.com/shop/view_shop.htm?spm=a1z0k.7628869.0.0.2a5a37deRiWnIl&shop_id=267446508
  * @LegalDeclaration ：本文档内容难免存在Bug，仅限于交流学习，禁止用于任何的商业用途
  * @Copyright   著作权归 “PowerX 功率探索” 所有
*****************************************************************************************/

#include "irq.h"  // 包含中断处理相关的头文件

uint16_t task500msFlag = 0;  // 500ms任务标志位
uint16_t task1000msFlag = 0; // 1000ms任务标志位

/**
  * @brief TIM6和DAC1中断服务函数
  * @note 此函数处理定时器6的中断，执行周期任务
  *       使用CCMRAM修饰符确保函数在CCM RAM中执行以提高性能
  */
CCMRAM void TIM6_DAC1_IRQHandler(void)
{
    static uint8_t i = 0;         // 用于分频计数的变量（0.1ms级）
    static uint16_t divCnt2 = 0;   // 2分频计数器（用于100us任务）
    static uint16_t divCnt5ms = 0; // 5ms分频计数器（用于通信任务）

    // 清除TIM6更新中断标志位
    TIM_ClearITPendingBit(TIM6, TIM_IT_Update);

    // 每10次中断增加1ms（10*100us=1ms）
    if (++i >= 10) {
        i = 0;
        g_systemTimeBaseMs += 1; // 系统时间基准增加1ms
    }

    // 执行保护检测任务（每100us执行一次）
    ProtectPoll();

    // 2分频处理（每200us执行一次）
    if (++divCnt2 >= 2) {
        divCnt2 = 0;
        // 执行参考值箝位
        RefValueClamp();
        // 执行逆变器开关处理
        Inv3pOnoffProcess();
        // 更新逆变器状态
        Inv3pStatusFresh();
    } else {
        // 执行滤波报告（每100us执行一次）
        ReportFilter();
    }

    // 每5ms执行一次通信任务
    if (((g_systemTimeBaseMs % 5) == 0) && (i == 0)) {
        if (++divCnt5ms >= 2) {
            divCnt5ms = 0;
            // 发送数据到PC
            SendDataToPc();
        } else {
            // 从PC接收数据
            ReceiveDataFromPC();
        }
    }
    
    // 设置500ms任务标志
    if (((g_systemTimeBaseMs % 500) == 0) && (i == 0)) {
        task500msFlag = 1;
    }
    
    // 设置1000ms任务标志
    if (((g_systemTimeBaseMs % 1000) == 0) && (i == 5)) {
        task1000msFlag = 1;
    }
}

/**
  * @brief DMA1通道1中断服务函数
  * @note 此函数处理DMA传输完成中断，执行控制环路
  *       使用CCMRAM修饰符确保函数在CCM RAM中执行以提高性能
  */
CCMRAM void DMA1_Channel1_IRQHandler(void)
{
    // 清除DMA1通道1传输完成中断标志
    DMA_ClearITPendingBit(DMA1_FLAG_TC1);

    // 执行逆变器三相控制环路
    Inv3pControlLoop();
}
