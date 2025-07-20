#ifndef __IRQ_H__
#define __IRQ_H__

#include "stm32f30x_conf.h"
#include "sys_timer.h"
#include "adc.h"
#include "gpio.h"
#include "inv3p.h"
#include "idle.h"
#include "protect.h"

extern uint16_t task500msFlag;
extern uint16_t task1000msFlag;

void TIM6_DAC1_IRQHandler(void);
void DMA1_Channel1_IRQHandler(void);

#endif
