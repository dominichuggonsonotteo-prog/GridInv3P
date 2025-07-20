/*-----------------------------------------------------------------------------
 * Name:    Joystick_STM32303E-EVAL.c
 * Purpose: ADC interface for STM32303E-EVAL board
 * Rev.:    1.0.0
 *----------------------------------------------------------------------------*/

/* Copyright (c) 2013 - 2017 ARM LIMITED

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

/*! \page stm32303e_eval_bsi_joystick Joystick BSI Setup

The BSI Joystick requires:
  - Setup of pins used for Joystick
 
Pinout tab
----------
  1. Click on <b>PC13</b> pin and select <b>GPIO_Input</b> function
  2. Click on <b>PE7</b> pin and select <b>GPIO_Input</b> function
  3. Click on <b>PD5</b> pin and select <b>GPIO_Input</b> function
  4. Click on <b>PB5</b> pin and select <b>GPIO_Input</b> function
  5. Click on <b>PD6/b> pin and select <b>GPIO_Input</b> function

Configuration tab
-----------------
  1. Under System open \b GPIO Configuration:
     - <b>GPIO</b>: Pull-Down on Jostick pins
          Pin Name | Signal on Pin | GPIO outp..|  GPIO mode  | GPIO Pull-up/Pull..| Maximum out |  Fast Mode  | User Label
          :--------|:--------------|:-----------|:------------|:-------------------|:------------|:------------|:----------
          PC13     | n/a           | n/a        | Input mode  | Pull-down..        | n/a         | n/a         
          PE7      | n/a           | n/a        | Input mode  | Pull-down..        | n/a         | n/a         
          PD5      | n/a           | n/a        | Input mode  | Pull-down..        | n/a         | n/a         
          PB5      | n/a           | n/a        | Input mode  | Pull-down..        | n/a         | n/a         
          PD6      | n/a           | n/a        | Input mode  | Pull-down..        | n/a         | n/a         
     Click \b OK to close the GPIO Configuration dialog

*/

#include "Board_Joystick.h"             // ::Board Support:Joystick

#include "stm32f3xx_hal.h"              // Device HAL
#include "MX_Device.h"

typedef struct {
  GPIO_TypeDef *port;
  uint16_t      pin;
  uint16_t      reserved;
} PIN_CONFIG_t;

/* ------------------------------- Configuration ---------------------------- */
#define JOYSTICK_COUNT                 (5)

/* Joystick pins:
   - center: PC13 (GPIO pin)
   - up:     PE7  (GPIO pin)
   - down:   PD5  (GPIO pin)
   - left:   PB5  (GPIO pin)
   - right:  PD6  (GPIO pin)                                                  */

#if ((!defined(MX_PC13_Pin))  || (!defined(MX_PE7_Pin)) ||                      \
     (!defined(MX_PD5_Pin)) || (!defined(MX_PB5_Pin)) || (!defined(MX_PD6_Pin)))
  #error "::Device:STM32Cube Framework (API): STM32CubeMX: Set pins PC13, PE7, PD5, PB5 and PD6 to GPIO_Output function!"
#endif

static const PIN_CONFIG_t PIN_CONFIG[JOYSTICK_COUNT] = {
  { MX_PC13_GPIOx, MX_PC13_GPIO_Pin, 0U },      /* Center                     */
  { MX_PE7_GPIOx,  MX_PE7_GPIO_Pin,  0U },      /* Up                         */
  { MX_PD5_GPIOx,  MX_PD5_GPIO_Pin,  0U },      /* Down                       */
  { MX_PB5_GPIOx,  MX_PB5_GPIO_Pin,  0U },      /* Left                       */
  { MX_PD6_GPIOx,  MX_PD6_GPIO_Pin,  0U }       /* Right                      */
};
/* -------------------------------------------------------------------------- */

/**
  \fn          int32_t Joystick_Initialize (void)
  \brief       Initialize joystick
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t Joystick_Initialize (void) {

  // All initialization code is generated by CubeMX
  return 0;
}

/**
  \fn          int32_t Joystick_Uninitialize (void)
  \brief       De-initialize joystick
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t Joystick_Uninitialize (void) {

  // All de-initialization code is generated by CubeMX
  return 0;
}

/**
  \fn          uint32_t Joystick_GetState (void)
  \brief       Get joystick state
  \returns     Joystick state
*/
uint32_t Joystick_GetState (void) {
  uint32_t val;

  val  = 0;
  if ((HAL_GPIO_ReadPin (PIN_CONFIG[0].port, PIN_CONFIG[0].pin)) == GPIO_PIN_SET) {val |= JOYSTICK_CENTER; }
  if ((HAL_GPIO_ReadPin (PIN_CONFIG[1].port, PIN_CONFIG[1].pin)) == GPIO_PIN_SET) { val |= JOYSTICK_UP; }
  if ((HAL_GPIO_ReadPin (PIN_CONFIG[2].port, PIN_CONFIG[2].pin)) == GPIO_PIN_SET) { val |= JOYSTICK_DOWN; }
  if ((HAL_GPIO_ReadPin (PIN_CONFIG[3].port, PIN_CONFIG[3].pin)) == GPIO_PIN_SET) { val |= JOYSTICK_LEFT; }
  if ((HAL_GPIO_ReadPin (PIN_CONFIG[4].port, PIN_CONFIG[4].pin)) == GPIO_PIN_SET) { val |= JOYSTICK_RIGHT; }

  return val;
}
