

#include "uart.h"  // 包含UART配置的头文件

// 全局变量定义：
PcCmdConfig g_cmdFromPC = {0};      // 从PC接收的解析后命令
PcCmdConfig g_cmdFromPCTemp = {0};  // 从PC接收的原始命令（DMA直接写入）
DataReportConfig g_sendToPC = {0};  // 发送到PC的数据报告

// uart初始化函数
void UartInit(void)
{		
    // 定义USART和DMA初始化结构体
    USART_InitTypeDef USART_InitStruct;
    DMA_InitTypeDef DMA_InitStructure;
    
    // 使能USART3和DMA1的时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);  // 使能USART3时钟
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);      // 使能DMA1时钟
    
    // 复位USART3寄存器到默认值
    USART_DeInit(USART3);
    
    // 配置USART3参数：
    USART_InitStruct.USART_BaudRate = 115200;                                         // 波特率115200
    USART_InitStruct.USART_StopBits = USART_StopBits_1;                               // 1位停止位
    USART_InitStruct.USART_Parity = USART_Parity_No;                                  // 无奇偶校验
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;                          // 8位数据长度
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                      // 使能接收和发送
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;      // 无硬件流控
    
    // 初始化USART3
    USART_Init(USART3, &USART_InitStruct);                                            // 应用配置
    // 配置DMA接收通道（通道3，用于USART3接收）：
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &(USART3->RDR);             // 外设地址：USART3接收数据寄存器
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) &g_cmdFromPCTemp;               // 内存地址：接收缓冲区
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                                // 传输方向：外设到内存
    DMA_InitStructure.DMA_BufferSize = 22;                                            // 缓冲区大小：22字节
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;                  // 外设地址不递增
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                           // 内存地址递增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;           // 外设数据大小：字节
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;                   // 内存数据大小：字节
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                                   // 循环模式（持续接收）
    DMA_InitStructure.DMA_Priority = DMA_Priority_Low;                                // 低优先级
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                                      // 禁用内存到内存模式
    
    // 初始化DMA通道3（USART3_RX）
    DMA_Init(DMA1_Channel3, &DMA_InitStructure);                                      // 应用配置
		
    // 使能DMA接收通道和USART3的DMA接收请求
    DMA_Cmd(DMA1_Channel3, ENABLE);                                                   // 使能DMA通道3
    USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);                                    // 使能USART3的DMA接收
    // 配置DMA发送通道（通道2，用于USART3发送）：
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &(USART3->TDR);             // 外设地址：USART3发送数据寄存器
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) &g_sendToPC;                    // 内存地址：发送缓冲区
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                                // 传输方向：内存到外设
    DMA_InitStructure.DMA_BufferSize = 30;                                            // 缓冲区大小：30字节
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;                  // 外设地址不递增
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                           // 内存地址递增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;           // 外设数据大小：字节
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;                   // 内存数据大小：字节
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                                     // 正常模式（单次传输）
    DMA_InitStructure.DMA_Priority = DMA_Priority_Low;                                // 低优先级
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                                      // 禁用内存到内存模式
    
    // 初始化DMA通道2（USART3_TX）
    DMA_Init(DMA1_Channel2, &DMA_InitStructure);                                      // 应用配置
		
    // 使能USART3的DMA发送请求并启用USART3
    USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE);                                    // 使能USART3的DMA发送
    USART_Cmd(USART3, ENABLE);                                                        // 使能USART3
}
