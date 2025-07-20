/****************************************************************************************
  * @file    iwdg.c
  * @author  PowerX 功率探索
  * @version V1.0.0
  * @date    01-July-2023
  * @淘宝店铺链接：https://gvmu6dghak5o8m0b3kv7j3j8bx49256.taobao.com/shop/view_shop.htm?spm=a1z0k.7628869.0.0.2a5a37deRiWnIl&shop_id=267446508
  * @LegalDeclaration ：本文档内容难免存在Bug，仅限于交流学习，禁止用于任何的商业用途
  * @Copyright   著作权归 "PowerX 功率探索" 所有
*****************************************************************************************/

#include "iwdg.h"  // 包含独立看门狗配置的头文件

/**
  * @brief  初始化独立看门狗(IWDG)
  * @note   配置看门狗的超时时间并启用，用于系统故障恢复
  *         计算说明：
  *         - LSI时钟频率 ≈ 40kHz
  *         - 预分频256，时钟频率 = 40kHz/256 ≈ 156.25Hz
  *         - 重载值312，超时时间 = 312/156.25 ≈ 2秒
  *         系统需要在2秒内至少喂狗一次，否则将触发复位
  */
void IwdgInit(void)
{
    // 使能对IWDG寄存器的写访问
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);   
    
    // 设置预分频器为256
    IWDG_SetPrescaler(IWDG_Prescaler_256);          
    
    // 设置重载值为312（约2秒超时）
    IWDG_SetReload(312);                            
    
    // 重载计数器（喂狗）
    IWDG_ReloadCounter();                           
    
    // 使能独立看门狗
    IWDG_Enable();                                  
}
