

#include "nvic.h"  // 包含NVIC配置的头文件

// 中断优先级配置函数
void NvicIrqSet(uint8_t channel, uint8_t priority, FunctionalState status, FunctionPointer IRQHandle) {
    // 检查中断服务函数指针是否有效
    if (IRQHandle == NULL) {
        return;  // 无效指针，直接返回
    }

    // 定义NVIC初始化结构体
    NVIC_InitTypeDef NVIC_InitStructure;

    // 设置NVIC优先级分组为4（4位抢占优先级，0位子优先级）
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    // 配置NVIC初始化参数：
    NVIC_InitStructure.NVIC_IRQChannel = channel;                     // 设置中断通道
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;                // 子优先级固定为0（分组4不使用子优先级）
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = priority;  // 设置抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = status;                   // 设置中断使能状态
    
    // 应用NVIC配置
    NVIC_Init(&NVIC_InitStructure);
}
