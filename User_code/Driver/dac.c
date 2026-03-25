

#include "dac.h"

// DAC初始化函数
void DacInit(void)
{
		DAC_InitTypeDef DAC_InitStructure;
		// 开启DAC的外设时钟
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
		// 复位DAC1外设
		DAC_DeInit(DAC1);
		// DAC1初始化
		DAC_InitStructure.DAC_Trigger = DAC_Trigger_None; // 无触发源模式
		DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None; // 禁能波形发生器模式
		DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bits2_0; // 禁能三角波功能
		DAC_InitStructure.DAC_Buffer_Switch = DAC_BufferSwitch_Disable; // 禁能DAC的buffer
		DAC_Init(DAC1, DAC_Channel_2, &DAC_InitStructure);
		// 使能DAC1的CH2
		DAC_Cmd(DAC1, DAC_Channel_2, ENABLE);
		// 根据比较器封波电流阈值设定DAC通道的输出电压
		DAC_SetChannel2Data(DAC1, DAC_Align_12b_R, CBC_INV3P_OF_DAC);
}