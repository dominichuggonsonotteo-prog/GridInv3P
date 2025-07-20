/****************************************************************************************
  * @file    main.c
  * @author  PowerX 功率探索
  * @version V1.0.0
  * @date    01-July-2023
  * @淘宝店铺链接：https://gvmu6dghak5o8m0b3kv7j3j8bx49256.taobao.com/shop/view_shop.htm?spm=a1z0k.7628869.0.0.2a5a37deRiWnIl&shop_id=267446508
  * @LegalDeclaration ：本文档内容难免存在Bug，仅限于交流学习，禁止用于任何的商业用途
  * @Copyright   著作权归 “PowerX 功率探索” 所有
*****************************************************************************************/

#include "stm32f30x_conf.h"
#include "sys_timer.h"
#include "gpio.h"
#include "nvic.h"
#include "adc.h"
#include "dac.h"
#include "comp.h"
#include "hrtimer.h"
#include "timer.h"
#include "uart.h"
#include "iwdg.h"
#include "irq.h"
#include "idle.h"
#include "oled.h"
#include "protect.h"
#include "main.h"

int main(void)
{
    // 主函数入口 - 初始化系统外设和模块
     
    GpioInit();  // 初始化GPIO引脚配置
     
    AdcInit();   // 初始化ADC模块
     
    DacInit();   // 初始化DAC模块
     
    CompInit();  // 初始化比较器模块
     
    TimInit();   // 初始化定时器模块
     
    HRTimInit(); // 初始化高分辨率定时器
     
    DelayMs(100); // 延时100ms确保外设稳定
     
    UartInit();  // 初始化串口通信
     
    OledInit();  // 初始化OLED显示屏
     
    IwdgInit();  // 初始化独立看门狗
     
    SampleParamInit(); // 初始化采样参数
     
    PllLoopParamInit(); // 初始化锁相环参数
     
    Inv3pLoopParamInit(); // 初始化三相逆变器控制参数

     
    SysTimInit(TIM6_DAC1_IRQHandler); // 初始化系统定时器并设置中断处理
     
    NvicIrqSet(DMA1_Channel1_IRQn, 0, ENABLE, DMA1_Channel1_IRQHandler); // 设置DMA通道1中断

    while (1) { // 主循环 - 执行周期性任务
         
        IWDG_ReloadCounter(); // 喂狗，防止看门狗复位
         
        // 500ms周期任务 - LED状态更新和风扇控制
        if (task500msFlag == 1) {
            task500msFlag = 0;
            LedBlink();
            FanStatusFresh();
        }
         
        // 1000ms周期任务 - OLED显示刷新
        if (task1000msFlag == 1) {
            task1000msFlag = 0;
            OledFresh();
        }
    }
}
