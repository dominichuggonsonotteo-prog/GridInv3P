#ifndef __NVIC_H__
#define __NVIC_H__

#include "stm32f30x_conf.h"
#include "irq.h"
#include "main.h"

void NvicIrqSet(uint8_t channel, uint8_t priority, FunctionalState status, FunctionPointer IRQHandle);

#endif
