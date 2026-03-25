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
    // GPIO初始化
    GpioInit();
    // ADC初始化
    AdcInit();
    // DAC初始化
    DacInit();
    // 比较器初始化
    CompInit();
    // 定时器初始化
    TimInit();
    // PWM初始化
    HRTimInit();
    // 延时100ms，稳定OLED供电
    DelayMs(100);
    // 串口初始化
    UartInit();
    // OLED初始化
    OledInit();
    // 看门狗初始化
    //IwdgInit();
    // 采样系数初始化
    SampleParamInit();
    // PLL的PI控制器初始化
    PllLoopParamInit();
    // 逆变器的PR控制器初始化
    Inv3pLoopParamInit();
    // 10k慢速中断初始化
    SysTimInit(TIM6_DAC1_IRQHandler);
    // 50k快速中断初始化
    NvicIrqSet(DMA1_Channel1_IRQn, 0, ENABLE, DMA1_Channel1_IRQHandler);
		
    while (1) {
        // 主循环喂狗
        //IWDG_ReloadCounter();
        // 500ms任务
        if (task500msFlag == 1) {
            task500msFlag = 0;
            LedBlink();
            FanStatusFresh();
        }
        // 1000ms任务
        if (task1000msFlag == 1) {
            task1000msFlag = 0;
            OledFresh();
        }
    }
}
