#ifndef __HRTIMER_H__
#define __HRTIMER_H__

#include "stm32f30x_conf.h"

 
#define HRTIM_PEROID 46080
 
#define DEAD_TIME    288
 
#define MIN_COMPARE  2000
 
#define MAX_COMPARE  (HRTIM_PEROID - MIN_COMPARE)
 
#define CONTROL_FREQ 25000
 
#define CONTROL_PEROID (0.00004f)

void HRTimInit(void);

#endif
