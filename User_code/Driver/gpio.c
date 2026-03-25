
		
#include "gpio.h"  // 包含GPIO配置的头文件
		
// GPIO初始化函数
void GpioInit(void)
{
    // 开启GPIO时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    
    // 使能GPIOA-GPIOF的AHB总线时钟（所有GPIO端口）
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOC | \
                          RCC_AHBPeriph_GPIOD | RCC_AHBPeriph_GPIOE | RCC_AHBPeriph_GPIOF, ENABLE);
    
    // 电源控制板的GPIO模式配置
    GpioInitConfig(GPIOC, GPIO_Pin_0, GPIO_Mode_AN, GPIO_PuPd_NOPULL); // Vac3
    GpioInitConfig(GPIOC, GPIO_Pin_1, GPIO_Mode_AN, GPIO_PuPd_NOPULL); // Vac2
    GpioInitConfig(GPIOC, GPIO_Pin_2, GPIO_Mode_AN, GPIO_PuPd_NOPULL); // Vac1
    GpioInitConfig(GPIOA, GPIO_Pin_0, GPIO_Mode_AN, GPIO_PuPd_NOPULL); // Iac3
    GpioInitConfig(GPIOA, GPIO_Pin_1, GPIO_Mode_AN, GPIO_PuPd_NOPULL); // Iac2
    GpioInitConfig(GPIOA, GPIO_Pin_2, GPIO_Mode_AN, GPIO_PuPd_NOPULL); // Iac1
    GpioInitConfig(GPIOA, GPIO_Pin_3, GPIO_Mode_AN, GPIO_PuPd_NOPULL); // Vdc1
    GpioInitConfig(GPIOA, GPIO_Pin_4, GPIO_Mode_AN, GPIO_PuPd_NOPULL); // Idc1
    GpioInitConfig(GPIOA, GPIO_Pin_7, GPIO_Mode_AN, GPIO_PuPd_NOPULL); // Icbc1
    GpioInitConfig(GPIOB, GPIO_Pin_2, GPIO_Mode_AN, GPIO_PuPd_NOPULL); // Vdc2
    GpioInitConfig(GPIOB, GPIO_Pin_1, GPIO_Mode_AN, GPIO_PuPd_NOPULL); // Idc2
    GpioInitConfig(GPIOB, GPIO_Pin_0, GPIO_Mode_AN, GPIO_PuPd_NOPULL); // Icbc2
    GpioInitConfig(GPIOC, GPIO_Pin_4, GPIO_Mode_AN, GPIO_PuPd_NOPULL); // Iref
    GpioInitConfig(GPIOC, GPIO_Pin_5, GPIO_Mode_AN, GPIO_PuPd_NOPULL); // Vref
    GpioInitConfig(GPIOA, GPIO_Pin_5, GPIO_Mode_AN, GPIO_PuPd_NOPULL); // Temp1
    GpioInitConfig(GPIOA, GPIO_Pin_6, GPIO_Mode_AN, GPIO_PuPd_NOPULL); // Temp2
		
    GpioInitConfig(GPIOC, GPIO_Pin_13, GPIO_Mode_OUT, GPIO_PuPd_DOWN); // LED1
    GpioInitConfig(GPIOC, GPIO_Pin_14, GPIO_Mode_OUT, GPIO_PuPd_DOWN); // LED2
    GpioInitConfig(GPIOC, GPIO_Pin_15, GPIO_Mode_OUT, GPIO_PuPd_DOWN); // LED3
    GpioInitConfig(GPIOA, GPIO_Pin_15, GPIO_Mode_IN, GPIO_PuPd_DOWN);  // ADDR1
    GpioInitConfig(GPIOB, GPIO_Pin_3, GPIO_Mode_IN, GPIO_PuPd_DOWN);   // ADDR2
    GpioInitConfig(GPIOB, GPIO_Pin_4, GPIO_Mode_IN, GPIO_PuPd_DOWN);   // ADDR3
    GpioInitConfig(GPIOB, GPIO_Pin_5, GPIO_Mode_IN, GPIO_PuPd_DOWN);   // ADDR4
    GpioInitConfig(GPIOC, GPIO_Pin_6, GPIO_Mode_IN, GPIO_PuPd_DOWN);   // KEY
    GpioInitConfig(GPIOC, GPIO_Pin_7, GPIO_Mode_IN, GPIO_PuPd_DOWN);   // PSU_EN
    GpioInitConfig(GPIOA, GPIO_Pin_12, GPIO_Mode_OUT, GPIO_PuPd_DOWN); // FAN
    GpioInitConfig(GPIOB, GPIO_Pin_11, GPIO_Mode_OUT, GPIO_PuPd_DOWN); // RELAY
    GpioInitConfig(GPIOC, GPIO_Pin_3, GPIO_Mode_OUT, GPIO_PuPd_DOWN);  // DEBUG
    GpioInitConfig(GPIOD, GPIO_Pin_2, GPIO_Mode_OUT, GPIO_PuPd_DOWN);  // SCL
    GpioInitConfig(GPIOC, GPIO_Pin_12, GPIO_Mode_OUT, GPIO_PuPd_DOWN); // SDA
    GpioInitConfig(GPIOB, GPIO_Pin_8, GPIO_Mode_AF, GPIO_PuPd_DOWN);   // CAN_RX
    GpioInitConfig(GPIOB, GPIO_Pin_9, GPIO_Mode_AF, GPIO_PuPd_DOWN);   // CAN_TX
    GpioInitConfig(GPIOB, GPIO_Pin_6, GPIO_Mode_AF, GPIO_PuPd_DOWN);   // SCI_TX1
    GpioInitConfig(GPIOB, GPIO_Pin_7, GPIO_Mode_AF, GPIO_PuPd_DOWN);   // SCI_RX1
    GpioInitConfig(GPIOC, GPIO_Pin_10, GPIO_Mode_AF, GPIO_PuPd_DOWN);  // SCI_TX3
    GpioInitConfig(GPIOC, GPIO_Pin_11, GPIO_Mode_AF, GPIO_PuPd_DOWN);  // SCI_RX3
    GpioInitConfig(GPIOB, GPIO_Pin_10, GPIO_Mode_AF, GPIO_PuPd_DOWN);  // BEEP
    GpioInitConfig(GPIOA, GPIO_Pin_8, GPIO_Mode_AF, GPIO_PuPd_DOWN);   // PWM_A1
    GpioInitConfig(GPIOA, GPIO_Pin_9, GPIO_Mode_AF, GPIO_PuPd_DOWN);   // PWM_A2
    GpioInitConfig(GPIOA, GPIO_Pin_10, GPIO_Mode_AF, GPIO_PuPd_DOWN);  // PWM_B1
    GpioInitConfig(GPIOA, GPIO_Pin_11, GPIO_Mode_AF, GPIO_PuPd_DOWN);  // PWM_B2
    GpioInitConfig(GPIOB, GPIO_Pin_12, GPIO_Mode_AF, GPIO_PuPd_DOWN);  // PWM_C1
    GpioInitConfig(GPIOB, GPIO_Pin_13, GPIO_Mode_AF, GPIO_PuPd_DOWN);  // PWM_C2
    GpioInitConfig(GPIOB, GPIO_Pin_14, GPIO_Mode_AF, GPIO_PuPd_DOWN);  // PWM_D1
    GpioInitConfig(GPIOB, GPIO_Pin_15, GPIO_Mode_AF, GPIO_PuPd_DOWN);  // PWM_D2
    GpioInitConfig(GPIOC, GPIO_Pin_8, GPIO_Mode_AF, GPIO_PuPd_DOWN);   // PWM_E1
    GpioInitConfig(GPIOC, GPIO_Pin_9, GPIO_Mode_AF, GPIO_PuPd_DOWN);   // PWM_E2
    
    // GPIO外设引脚功能配置
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_9);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_9);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_7);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_7);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_7);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_7);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_13);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_13);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_13);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_13);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF_13);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_13);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_13);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_13);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_3);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_3);
		
    // LED灯默认熄灭
    LED_RED_OFF;
    LED_ORANGE_OFF;
     
    // 默认不开启风扇
    FAN_OFF;
}


// GPIO模式配置函数
void GpioInitConfig(GPIO_TypeDef * GPIO_Type, uint32_t GPIO_Pin, GPIOMode_TypeDef GPIO_Mode, GPIOPuPd_TypeDef GPIO_PuPd_Mode)
{
    GPIO_InitTypeDef GPIO_InitStructure;  // GPIO初始化结构体

    // 配置GPIO参数：
    GPIO_InitStructure.GPIO_PuPd  =  GPIO_PuPd_Mode;  // 上拉/下拉模式
    GPIO_InitStructure.GPIO_Speed =  GPIO_Speed_50MHz; // 最大输出速度50MHz（高速）
    GPIO_InitStructure.GPIO_Mode  =  GPIO_Mode;        // 引脚模式（输入/输出/复用/模拟）
    GPIO_InitStructure.GPIO_OType =  GPIO_OType_PP;    // 输出类型：推挽输出（开漏可选GPIO_OType_OD）
    GPIO_InitStructure.GPIO_Pin   =  GPIO_Pin;         // 要配置的引脚
    
    // 使用配置初始化GPIO
    GPIO_Init(GPIO_Type, &GPIO_InitStructure);
}


// GPIO电平翻转函数
void GpioOutputToggle(GPIO_TypeDef * GPIO_Type, uint32_t GPIO_Pin)
{
    // 读取当前输出状态
    if(GPIO_ReadOutputDataBit(GPIO_Type, GPIO_Pin)) {
      // 当前为高电平，置为低电平（关闭）
      GPIO_ResetBits(GPIO_Type, GPIO_Pin);
    } else {
      // 当前为低电平，置为高电平（开启）
      GPIO_SetBits(GPIO_Type, GPIO_Pin);
    }
}
