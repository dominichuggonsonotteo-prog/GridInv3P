
#include "comp.h"
// 比较器初始化，用于电感电流的CBC封波
void CompInit(void)
{
COMP_InitTypeDef COMP_InitStructure;
// 比较器4初始化
COMP_InitStructure.COMP_InvertingInput = COMP_InvertingInput_DAC1OUT2; // 比较器的负极性为DAC1CH2
COMP_InitStructure.COMP_NonInvertingInput = COMP_NonInvertingInput_IO1; // 比较器的正极性为电感电流采样引脚
COMP_InitStructure.COMP_Output = COMP_Output_HRTIM1_EE2_2; // 比较器的输出连接到HRTIM的外部事件2
COMP_InitStructure.COMP_BlankingSrce = COMP_BlankingSrce_None; // 不使能比较器的窗口
COMP_InitStructure.COMP_OutputPol = COMP_OutputPol_NonInverted; // 比较器的输出为正极性
COMP_InitStructure.COMP_Hysteresis = COMP_Hysteresis_No; // 不使能比较器的滞环功能
COMP_InitStructure.COMP_Mode = COMP_Mode_HighSpeed; // 比较器高速模式
COMP_Init(COMP_Selection_COMP4, &COMP_InitStructure);
// 使能比较器4
COMP_Cmd(COMP_Selection_COMP4, ENABLE);
}