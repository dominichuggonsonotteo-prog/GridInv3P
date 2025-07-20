#ifndef __DAC_H__
#define __DAC_H__

#include "stm32f30x_conf.h"

 
#define CBC_INV3P_VALUE   (13.0f)
 
#define CBC_INV3P_OF_DAC  (uint16_t)(CBC_INV3P_VALUE * (-160.6f) + (2007.5f))

void DacInit(void);

#endif
