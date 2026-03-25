

#include "iwdg.h"  // 包含独立看门狗配置的头文件

// 看门狗函数初始化
// 看门狗时钟为40kHz 256分频，计数达到312后复位，约2s
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
