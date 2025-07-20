/****************************************************************************************
  * @file    gpio.c
  * @author  PowerX 功率探索
  * @version V1.0.0
  * @date    01-July-2023
  * @淘宝店铺链接：https://gvmu6dghak5o8m0b3kv7j3j8bx49256.taobao.com/shop/view_shop.htm?spm=a1z0k.7628869.0.0.2a5a37deRiWnIl&shop_id=267446508
  * @LegalDeclaration ：本文档内容难免存在Bug，仅限于交流学习，禁止用于任何的商业用途
  * @Copyright   著作权归 "PowerX 功率探索" 所有
*****************************************************************************************/
		
#include "gpio.h"  // 包含GPIO配置的头文件
		
/**
  * @brief  初始化所有GPIO引脚
  * @note   配置系统时钟、GPIO模式、复用功能和初始状态
  *         此函数负责配置整个系统的GPIO引脚，包括：
  *         - 使能SYSCFG和GPIOA-GPIOF的时钟
  *         - 配置各种模拟输入引脚（用于ADC等）
  *         - 配置通用输出引脚（控制LED等）
  *         - 配置输入引脚（检测开关状态等）
  *         - 配置复用功能引脚（用于外设如USART、SPI等）
  *         - 设置GPIO复用功能映射
  *         - 初始化LED和风扇状态
  */
void GpioInit(void)
{
    // 使能SYSCFG时钟（系统配置控制器，用于复用功能配置）
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    
    // 使能GPIOA-GPIOF的AHB总线时钟（所有GPIO端口）
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOC | \
                          RCC_AHBPeriph_GPIOD | RCC_AHBPeriph_GPIOE | RCC_AHBPeriph_GPIOF, ENABLE);
    
    // 配置所有GPIO引脚模式：
    
    // 1. 模拟输入模式（无上拉下拉） - 用于ADC采样
    GpioInitConfig(GPIOC, GPIO_Pin_0, GPIO_Mode_AN, GPIO_PuPd_NOPULL);  // PC0: 模拟输入1
    GpioInitConfig(GPIOC, GPIO_Pin_1, GPIO_Mode_AN, GPIO_PuPd_NOPULL);  // PC1: 模拟输入2
    GpioInitConfig(GPIOC, GPIO_Pin_2, GPIO_Mode_AN, GPIO_PuPd_NOPULL);  // PC2: 模拟输入3
    GpioInitConfig(GPIOA, GPIO_Pin_0, GPIO_Mode_AN, GPIO_PuPd_NOPULL);  // PA0: 模拟输入4
    GpioInitConfig(GPIOA, GPIO_Pin_1, GPIO_Mode_AN, GPIO_PuPd_NOPULL);  // PA1: 模拟输入5
    GpioInitConfig(GPIOA, GPIO_Pin_2, GPIO_Mode_AN, GPIO_PuPd_NOPULL);  // PA2: 模拟输入6
    GpioInitConfig(GPIOA, GPIO_Pin_3, GPIO_Mode_AN, GPIO_PuPd_NOPULL);  // PA3: 模拟输入7
    GpioInitConfig(GPIOA, GPIO_Pin_4, GPIO_Mode_AN, GPIO_PuPd_NOPULL);  // PA4: 模拟输入8
    GpioInitConfig(GPIOA, GPIO_Pin_7, GPIO_Mode_AN, GPIO_PuPd_NOPULL);  // PA7: 模拟输入9
    GpioInitConfig(GPIOB, GPIO_Pin_2, GPIO_Mode_AN, GPIO_PuPd_NOPULL);  // PB2: 模拟输入10
    GpioInitConfig(GPIOB, GPIO_Pin_1, GPIO_Mode_AN, GPIO_PuPd_NOPULL);  // PB1: 模拟输入11
    GpioInitConfig(GPIOB, GPIO_Pin_0, GPIO_Mode_AN, GPIO_PuPd_NOPULL);  // PB0: 模拟输入12
    GpioInitConfig(GPIOC, GPIO_Pin_4, GPIO_Mode_AN, GPIO_PuPd_NOPULL);  // PC4: 模拟输入13
    GpioInitConfig(GPIOC, GPIO_Pin_5, GPIO_Mode_AN, GPIO_PuPd_NOPULL);  // PC5: 模拟输入14
    GpioInitConfig(GPIOA, GPIO_Pin_5, GPIO_Mode_AN, GPIO_PuPd_NOPULL);  // PA5: 模拟输入15
    GpioInitConfig(GPIOA, GPIO_Pin_6, GPIO_Mode_AN, GPIO_PuPd_NOPULL);  // PA6: 模拟输入16
    
    // 2. 输出模式（下拉） - 用于LED控制
    GpioInitConfig(GPIOC, GPIO_Pin_13, GPIO_Mode_OUT, GPIO_PuPd_DOWN);  // PC13: 黄色LED控制
    GpioInitConfig(GPIOC, GPIO_Pin_14, GPIO_Mode_OUT, GPIO_PuPd_DOWN);  // PC14: 蓝色LED控制
    GpioInitConfig(GPIOC, GPIO_Pin_15, GPIO_Mode_OUT, GPIO_PuPd_DOWN);  // PC15: 备用LED控制
    
    // 3. 输入模式（下拉） - 用于检测开关状态
    GpioInitConfig(GPIOA, GPIO_Pin_15, GPIO_Mode_IN, GPIO_PuPd_DOWN);   // PA15: 开关输入1
    GpioInitConfig(GPIOB, GPIO_Pin_3, GPIO_Mode_IN, GPIO_PuPd_DOWN);    // PB3: 开关输入2
    GpioInitConfig(GPIOB, GPIO_Pin_4, GPIO_Mode_IN, GPIO_PuPd_DOWN);    // PB4: 开关输入3
    GpioInitConfig(GPIOB, GPIO_Pin_5, GPIO_Mode_IN, GPIO_PuPd_DOWN);    // PB5: 开关输入4
    GpioInitConfig(GPIOC, GPIO_Pin_6, GPIO_Mode_IN, GPIO_PuPd_DOWN);    // PC6: 开关输入5
    GpioInitConfig(GPIOC, GPIO_Pin_7, GPIO_Mode_IN, GPIO_PuPd_DOWN);    // PC7: 开关输入6
    
    // 4. 输出模式（下拉） - 控制引脚（继电器、风扇等）
    GpioInitConfig(GPIOA, GPIO_Pin_12, GPIO_Mode_OUT, GPIO_PuPd_DOWN);  // PA12: 继电器控制1
    GpioInitConfig(GPIOB, GPIO_Pin_11, GPIO_Mode_OUT, GPIO_PuPd_DOWN);  // PB11: 继电器控制2
    GpioInitConfig(GPIOC, GPIO_Pin_3, GPIO_Mode_OUT, GPIO_PuPd_DOWN);   // PC3: 风扇控制
    GpioInitConfig(GPIOD, GPIO_Pin_2, GPIO_Mode_OUT, GPIO_PuPd_DOWN);   // PD2: 备用控制1
    GpioInitConfig(GPIOC, GPIO_Pin_12, GPIO_Mode_OUT, GPIO_PuPd_DOWN);  // PC12: 备用控制2
    
    // 5. 复用功能模式（下拉） - 用于外设功能（USART, SPI, I2C等）
    GpioInitConfig(GPIOB, GPIO_Pin_8, GPIO_Mode_AF, GPIO_PuPd_DOWN);    // PB8: I2C1_SCL或TIM4_CH3
    GpioInitConfig(GPIOB, GPIO_Pin_9, GPIO_Mode_AF, GPIO_PuPd_DOWN);    // PB9: I2C1_SDA或TIM4_CH4
    GpioInitConfig(GPIOB, GPIO_Pin_6, GPIO_Mode_AF, GPIO_PuPd_DOWN);    // PB6: I2C1_SCL或USART1_TX
    GpioInitConfig(GPIOB, GPIO_Pin_7, GPIO_Mode_AF, GPIO_PuPd_DOWN);    // PB7: I2C1_SDA或USART1_RX
    GpioInitConfig(GPIOC, GPIO_Pin_10, GPIO_Mode_AF, GPIO_PuPd_DOWN);   // PC10: UART4_TX或SPI3_SCK
    GpioInitConfig(GPIOC, GPIO_Pin_11, GPIO_Mode_AF, GPIO_PuPd_DOWN);   // PC11: UART4_RX或SPI3_MISO
    GpioInitConfig(GPIOB, GPIO_Pin_10, GPIO_Mode_AF, GPIO_PuPd_DOWN);   // PB10: I2C2_SCL或USART3_TX
    GpioInitConfig(GPIOA, GPIO_Pin_8, GPIO_Mode_AF, GPIO_PuPd_DOWN);    // PA8: TIM1_CH1或MCO
    GpioInitConfig(GPIOA, GPIO_Pin_9, GPIO_Mode_AF, GPIO_PuPd_DOWN);    // PA9: USART1_TX或TIM1_CH2
    GpioInitConfig(GPIOA, GPIO_Pin_10, GPIO_Mode_AF, GPIO_PuPd_DOWN);   // PA10: USART1_RX或TIM1_CH3
    GpioInitConfig(GPIOA, GPIO_Pin_11, GPIO_Mode_AF, GPIO_PuPd_DOWN);   // PA11: USART1_CTS或TIM1_CH4
    GpioInitConfig(GPIOB, GPIO_Pin_12, GPIO_Mode_AF, GPIO_PuPd_DOWN);   // PB12: SPI2_NSS或I2C2_SMBA
    GpioInitConfig(GPIOB, GPIO_Pin_13, GPIO_Mode_AF, GPIO_PuPd_DOWN);   // PB13: SPI2_SCK或TIM1_CH1N
    GpioInitConfig(GPIOB, GPIO_Pin_14, GPIO_Mode_AF, GPIO_PuPd_DOWN);   // PB14: SPI2_MISO或TIM1_CH2N
    GpioInitConfig(GPIOB, GPIO_Pin_15, GPIO_Mode_AF, GPIO_PuPd_DOWN);   // PB15: SPI2_MOSI或TIM1_CH3N
    GpioInitConfig(GPIOC, GPIO_Pin_8, GPIO_Mode_AF, GPIO_PuPd_DOWN);    // PC8: TIM3_CH3或SDIO_D0
    GpioInitConfig(GPIOC, GPIO_Pin_9, GPIO_Mode_AF, GPIO_PuPd_DOWN);    // PC9: TIM3_CH4或SDIO_D1
    
    // 配置GPIO复用功能映射：
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_9);   // PB8: 复用为AF9 (TIM4_CH3)
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_9);   // PB9: 复用为AF9 (TIM4_CH4)
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_7);   // PB6: 复用为AF7 (I2C1_SCL)
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_7);   // PB7: 复用为AF7 (I2C1_SDA)
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_7);  // PC10: 复用为AF7 (UART4_TX)
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_7);  // PC11: 复用为AF7 (UART4_RX)
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_1);  // PB10: 复用为AF1 (TIM2_CH3)
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_13);  // PA8: 复用为AF13 (HRTIM1_CHA2)
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_13);  // PA9: 复用为AF13 (HRTIM1_CHB1)
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_13); // PA10: 复用为AF13 (HRTIM1_CHB2)
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_13); // PA11: 复用为AF13 (HRTIM1_CHC1)
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF_13); // PB12: 复用为AF13 (HRTIM1_CHD1)
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_13); // PB13: 复用为AF13 (HRTIM1_CHD2)
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_13); // PB14: 复用为AF13 (HRTIM1_CHC2)
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_13); // PB15: 复用为AF13 (HRTIM1_CHC1)
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_3);   // PC8: 复用为AF3 (TIM3_CH3)
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_3);   // PC9: 复用为AF3 (TIM3_CH4)
		
    // 初始化LED状态
    LED_YELLOW_OFF;  // 关闭黄色LED（初始状态）
    LED_BLUE_OFF;    // 关闭蓝色LED（初始状态）
     
    FAN_OFF;  // 关闭风扇（初始状态）
}


/**
  * @brief  配置单个GPIO引脚
  * @param  GPIO_Type: GPIO端口（GPIOA, GPIOB等）
  * @param  GPIO_Pin: 引脚号（使用GPIO_Pin_x宏）
  * @param  GPIO_Mode: 引脚模式（输入、输出、复用等）
  * @param  GPIO_PuPd_Mode: 上拉/下拉配置
  * @note   此函数初始化单个GPIO引脚，设置其工作模式、输出类型、速度和上下拉配置
  */
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


/**
  * @brief  切换GPIO输出状态
  * @param  GPIO_Type: GPIO端口
  * @param  GPIO_Pin: 要切换的引脚
  * @note   此函数切换指定引脚的输出状态：
  *         - 如果当前为高电平则置低
  *         - 如果为低电平则置高
  *         常用于LED闪烁、继电器切换等场景
  */
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
