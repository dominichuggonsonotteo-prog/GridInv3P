#ifndef __TIMER_H__
#define __TIMER_H__

#include "stm32f30x_conf.h"

 
#define BEEP_ON  TIM_Cmd(TIM2, ENABLE);
 
#define BEEP_OFF TIM_Cmd(TIM2, DISABLE);

void TimInit (void);
void DelayUs(uint16_t time);
void DelayMs(uint16_t nCount);

#endif
