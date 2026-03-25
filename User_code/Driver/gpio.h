#ifndef __GPIO_H__
#define __GPIO_H__

#include "stm32f30x_conf.h"


#define LED_RED_ON      (GPIO_SetBits(GPIOC, GPIO_Pin_15)) //故障指示灯
#define LED_RED_OFF     (GPIO_ResetBits(GPIOC, GPIO_Pin_15))
#define LED_ORANGE_ON        (GPIO_SetBits(GPIOC, GPIO_Pin_14))//运行指示灯
#define LED_ORANGE_OFF       (GPIO_ResetBits(GPIOC, GPIO_Pin_14))



 
#define FAN_ON             (GPIO_SetBits(GPIOA, GPIO_Pin_12))//并没有发挥作用
#define FAN_OFF            (GPIO_ResetBits(GPIOA, GPIO_Pin_12))//并没有发挥作用
 
#define DEBUG_ON           (GPIO_SetBits(GPIOC, GPIO_Pin_3))//并没有发挥作用
#define DEBUG_OFF          (GPIO_ResetBits(GPIOC, GPIO_Pin_3))//并没有发挥作用
 
#define SWITCH_STATUS      (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7))//并没有发挥作用
 
#define KEY_STATUS         (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6))//并没有发挥作用

void GpioInit(void);
void GpioInitConfig(GPIO_TypeDef * GPIO_Type, uint32_t GPIO_Pin, GPIOMode_TypeDef GPIO_Mode, GPIOPuPd_TypeDef GPIO_PuPd_Mode);
void GpioOutputToggle(GPIO_TypeDef * GPIO_Type, uint32_t GPIO_Pin);

#endif
