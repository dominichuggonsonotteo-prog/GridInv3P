/*-----------------------------------------------------------------------------
 * Name:    LED_746G_Discovery.c
 * Purpose: LED interface for STM32F746G Discovery Kit
 * Rev.:    1.0.2
 *----------------------------------------------------------------------------*/

/* Copyright (c) 2015 - 2017 ARM LIMITED

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ARM nor the names of its contributors may be used
     to endorse or promote products derived from this software without
     specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/

/* Using the BSP LED driver for STM32 CubeMX targets
 
- In the CubeMX Pinout tab configure the pins connected to LEDs as GPIO_Output. 
- Select "Enter User Label" from the context menu of the pin
- Name the buttons LD1 through LD10 as available. 

This driver will pick up the names automatically after you generated code.
 
*/

#include "stm32f3xx_hal.h"           // Needs define in RTE component

#include "Board_LED.h"

/* GPIO Pin identifier */
typedef struct _GPIO_PIN {
  GPIO_TypeDef *port;
  uint16_t      pin;
  uint16_t      reserved;
} GPIO_PIN;

/* LED GPIO Pins */
static const GPIO_PIN LED_PIN[] = {
#ifdef LD1_Pin
{ LD1_GPIO_Port, LD1_Pin, 0U },
#endif	
#ifdef LD2_Pin
{ LD2_GPIO_Port, LD2_Pin, 0U },
#endif	
#ifdef LD3_Pin
{ LD3_GPIO_Port, LD3_Pin, 0U },
#endif	
#ifdef LD4_Pin
{ LD4_GPIO_Port, LD4_Pin, 0U },
#endif	
#ifdef LD5_Pin
{ LD5_GPIO_Port, LD5_Pin, 0U },
#endif	
#ifdef LD6_Pin
{ LD6_GPIO_Port, LD6_Pin, 0U },
#endif	
#ifdef LD7_Pin
{ LD7_GPIO_Port, LD7_Pin, 0U },
#endif	
#ifdef LD8_Pin
{ LD8_GPIO_Port, LD8_Pin, 0U },
#endif	
#ifdef LD9_Pin
{ LD9_GPIO_Port, LD9_Pin, 0U },
#endif	
#ifdef LD10_Pin
{ LD10_GPIO_Port, LD10_Pin, 0U },
#endif	

};

#define LED_COUNT (sizeof(LED_PIN)/sizeof(GPIO_PIN))


/**
  \fn          int32_t LED_Initialize (void)
  \brief       Initialize LEDs
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t LED_Initialize (void) {

  return 0;
}

/**
  \fn          int32_t LED_Uninitialize (void)
  \brief       De-initialize LEDs
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t LED_Uninitialize (void) {

    return 0;
}

/**
  \fn          int32_t LED_On (uint32_t num)
  \brief       Turn on requested LED
  \param[in]   num  LED number
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t LED_On (uint32_t num) {
  if (num < LED_COUNT) {
    HAL_GPIO_WritePin(LED_PIN[num].port, LED_PIN[num].pin, GPIO_PIN_SET);
  }
  return 0;
}

/**
  \fn          int32_t LED_Off (uint32_t num)
  \brief       Turn off requested LED
  \param[in]   num  LED number
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t LED_Off (uint32_t num) {
  if (num < LED_COUNT) {
    HAL_GPIO_WritePin(LED_PIN[num].port, LED_PIN[num].pin, GPIO_PIN_RESET);
  }
  return 0;
}

/**
  \fn          int32_t LED_SetOut (uint32_t val)
  \brief       Write value to LEDs
  \param[in]   val  value to be displayed on LEDs
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t LED_SetOut (uint32_t val) {
  uint32_t n;

  for (n = 0; n < LED_COUNT; n++) {
    if (val & (1 << n)) LED_On (n);
    else                LED_Off(n);
  }
  return 0;
}

/**
  \fn          uint32_t LED_GetCount (void)
  \brief       Get number of LEDs
  \return      Number of available LEDs
*/
uint32_t LED_GetCount (void) {
  return LED_COUNT;
}
