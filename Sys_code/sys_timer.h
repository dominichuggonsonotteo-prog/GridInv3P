#ifndef __SYS_TIMER_H__
#define __SYS_TIMER_H__

#include "stm32f30x_conf.h"
#include "gpio.h"
#include "nvic.h"
#include "main.h"

extern uint32_t g_systemTimeBaseMs;

void SysTimInit(FunctionPointer IRQHandle);

#endif
