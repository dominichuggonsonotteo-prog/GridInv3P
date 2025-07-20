#ifndef __OLED_H__
#define __OLED_H__

#include "stm32f30x_conf.h"


#define OLED_CMD      0     
#define OLED_DATA     1     
#define Max_Column    128

#define OLED_SCLK_Clr GPIO_ResetBits(GPIOD, GPIO_Pin_2)   
#define OLED_SCLK_Set GPIO_SetBits(GPIOD, GPIO_Pin_2)

#define OLED_SDIN_Clr GPIO_ResetBits(GPIOC, GPIO_Pin_12)  
#define OLED_SDIN_Set GPIO_SetBits(GPIOC, GPIO_Pin_12)

void OledWrByte(unsigned dat, unsigned cmd);
void WriteIicCommand(unsigned IIC_Command);
void WriteIicData(unsigned IIC_Data);
void IicStart(void);
void WriteIicByte(unsigned char IIC_Byte);
void IicWaitAck(void);
void IicStop(void);
void OledClear(void) ;
void OledShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size2);
void OledShowString(uint8_t x, uint8_t y, uint8_t * chr, uint8_t Char_Size);
void OledShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t Char_Size);
void OledSetPos(unsigned x, unsigned y) ;
uint32_t OledPow(uint8_t m, uint8_t n);
void OledOn(void);
void OledDisplayOn(void);
void OledDisplayOff(void);
void OledInit(void);

#endif
