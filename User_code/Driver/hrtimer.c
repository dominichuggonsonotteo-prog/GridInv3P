

#include "hrtimer.h"  // 包含高分辨率定时器配置的头文件

// HRTIMER初始化函数
void HRTimInit(void)
{
    // 定义HRTIM配置结构体
    HRTIM_OutputCfgTypeDef HRTIM_TIM_OutputStructure;
    HRTIM_BaseInitTypeDef HRTIM_BaseInitStructure;
    HRTIM_TimerInitTypeDef HRTIM_TimerInitStructure;
    HRTIM_TimerCfgTypeDef HRTIM_TimerWaveStructure;
    HRTIM_ADCTriggerCfgTypeDef HRTIM_ADCTrigStructure;
    HRTIM_EventCfgTypeDef HRTIM_TIM_EVENT_Structure;
		
    // 配置HRTIM1时钟源为PLL
    RCC_HRTIM1CLKConfig(RCC_HRTIM1CLK_PLLCLK);
    
    // 使能HRTIM1的APB2总线时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_HRTIM1, ENABLE);
     
    // 启动DLL校准，校准速率为14
    HRTIM_DLLCalibrationStart(HRTIM1, HRTIM_CALIBRATIONRATE_14);                      
    // 使能DLL校准
    HRTIM1_COMMON->DLLCR |= HRTIM_DLLCR_CALEN;                                        
    // 等待DLL校准完成
    while(HRTIM_GetCommonFlagStatus(HRTIM1, HRTIM_ISR_DLLRDY) == RESET);              
		
    // 配置定时器初始化参数：
    HRTIM_TimerInitStructure.HalfModeEnable = HRTIM_HALFMODE_DISABLED;                // 禁用半模式
    HRTIM_TimerInitStructure.StartOnSync = HRTIM_SYNCSTART_DISABLED;                  // 禁用同步启动
    HRTIM_TimerInitStructure.ResetOnSync = HRTIM_SYNCRESET_DISABLED;                  // 禁用同步复位
    HRTIM_TimerInitStructure.DACSynchro = HRTIM_DACSYNC_NONE;                         // 无DAC同步
    HRTIM_TimerInitStructure.PreloadEnable = HRTIM_PRELOAD_ENABLED;                   // 使能预加载
    HRTIM_TimerInitStructure.UpdateGating = HRTIM_UPDATEGATING_INDEPENDENT;           // 独立更新门控
    HRTIM_TimerInitStructure.BurstMode = HRTIM_TIMERBURSTMODE_MAINTAINCLOCK;          // 突发模式维持时钟
    HRTIM_TimerInitStructure.RepetitionUpdate = HRTIM_UPDATEONREPETITION_ENABLED;     // 重复更新使能
     
    // 配置基础初始化参数：
    HRTIM_BaseInitStructure.Period = HRTIM_PEROID;                                    // 设置定时器周期
    HRTIM_BaseInitStructure.RepetitionCounter = 0;                                    // 重复计数器为0
    HRTIM_BaseInitStructure.PrescalerRatio = HRTIM_PRESCALERRATIO_MUL8;               // 预分频比率x8
    HRTIM_BaseInitStructure.Mode = HRTIM_MODE_CONTINOUS;                              // 连续模式
     
    // 初始化主定时器和三个从定时器(TIMER_C, TIMER_D, TIMER_E)
    HRTIM_Waveform_Init(HRTIM1, HRTIM_TIMERINDEX_MASTER, &HRTIM_BaseInitStructure, &HRTIM_TimerInitStructure);    
    HRTIM_Waveform_Init(HRTIM1, HRTIM_TIMERINDEX_TIMER_C, &HRTIM_BaseInitStructure, &HRTIM_TimerInitStructure);
    HRTIM_Waveform_Init(HRTIM1, HRTIM_TIMERINDEX_TIMER_D, &HRTIM_BaseInitStructure, &HRTIM_TimerInitStructure);
    HRTIM_Waveform_Init(HRTIM1, HRTIM_TIMERINDEX_TIMER_E, &HRTIM_BaseInitStructure, &HRTIM_TimerInitStructure);
    
     
    // 配置定时器波形参数：
    HRTIM_TimerWaveStructure.DeadTimeInsertion = HRTIM_TIMDEADTIMEINSERTION_DISABLED;        // 禁用死区时间插入
    HRTIM_TimerWaveStructure.DelayedProtectionMode = HRTIM_TIMDELAYEDPROTECTION_DISABLED;    // 禁用延迟保护模式
    HRTIM_TimerWaveStructure.FaultEnable = HRTIM_TIMFAULTENABLE_NONE;                        // 无故障使能
    HRTIM_TimerWaveStructure.FaultLock = HRTIM_TIMFAULTLOCK_READWRITE;                       // 故障锁定可读写
    HRTIM_TimerWaveStructure.PushPull = HRTIM_TIMPUSHPULLMODE_DISABLED;                      // 禁用推挽模式
    HRTIM_TimerWaveStructure.ResetTrigger = HRTIM_TIMRESETTRIGGER_MASTER_PER;                // 主定时器周期复位触发
    HRTIM_TimerWaveStructure.ResetUpdate = HRTIM_TIMUPDATEONRESET_ENABLED;                   // 复位时更新使能
    HRTIM_TimerWaveStructure.UpdateTrigger = HRTIM_TIMUPDATETRIGGER_MASTER;                  // 主定时器更新触发
    
    // 配置定时器C、D、E的波形参数
    HRTIM_WaveformTimerConfig(HRTIM1, HRTIM_TIMERINDEX_TIMER_C, &HRTIM_TimerWaveStructure);
    HRTIM_WaveformTimerConfig(HRTIM1, HRTIM_TIMERINDEX_TIMER_D, &HRTIM_TimerWaveStructure);
    HRTIM_WaveformTimerConfig(HRTIM1, HRTIM_TIMERINDEX_TIMER_E, &HRTIM_TimerWaveStructure);
    
    // 配置输出通道参数（TC2, TD2, TE2）：
    HRTIM_TIM_OutputStructure.Polarity = HRTIM_OUTPUTPOLARITY_HIGH;                                   // 输出极性高
    HRTIM_TIM_OutputStructure.SetSource = HRTIM_OUTPUTSET_TIMCMP2;                                    // 设置源为TIMCMP2
    HRTIM_TIM_OutputStructure.ResetSource = HRTIM_OUTPUTRESET_TIMCMP3;                                // 复位源为TIMCMP3
    HRTIM_TIM_OutputStructure.IdleMode = HRTIM_OUTPUTIDLEMODE_NONE;                                   // 无空闲模式
    HRTIM_TIM_OutputStructure.IdleState = HRTIM_OUTPUTIDLESTATE_INACTIVE;                             // 空闲状态为无效
    HRTIM_TIM_OutputStructure.FaultState = HRTIM_OUTPUTFAULTSTATE_INACTIVE;                           // 故障状态为无效
    HRTIM_TIM_OutputStructure.ChopperModeEnable = HRTIM_OUTPUTCHOPPERMODE_DISABLED;                   // 禁用斩波模式
    HRTIM_TIM_OutputStructure.BurstModeEntryDelayed = HRTIM_OUTPUTBURSTMODEENTRY_REGULAR;             // 常规突发模式入口
    
    // 配置定时器C、D、E的输出通道2
    HRTIM_WaveformOutputConfig(HRTIM1, HRTIM_TIMERINDEX_TIMER_C, HRTIM_OUTPUT_TC2, &HRTIM_TIM_OutputStructure);
    HRTIM_WaveformOutputConfig(HRTIM1, HRTIM_TIMERINDEX_TIMER_D, HRTIM_OUTPUT_TD2, &HRTIM_TIM_OutputStructure);
    HRTIM_WaveformOutputConfig(HRTIM1, HRTIM_TIMERINDEX_TIMER_E, HRTIM_OUTPUT_TE2, &HRTIM_TIM_OutputStructure);
     
    // 配置输出通道参数（TC1, TD1, TE1）：
    HRTIM_TIM_OutputStructure.Polarity = HRTIM_OUTPUTPOLARITY_HIGH;                                   // 输出极性高
    HRTIM_TIM_OutputStructure.SetSource = HRTIM_OUTPUTSET_TIMCMP4;                                    // 设置源为TIMCMP4
    HRTIM_TIM_OutputStructure.ResetSource = HRTIM_OUTPUTRESET_TIMCMP1;                                // 复位源为TIMCMP1
    HRTIM_TIM_OutputStructure.IdleMode = HRTIM_OUTPUTIDLEMODE_NONE;                                   // 无空闲模式
    HRTIM_TIM_OutputStructure.IdleState = HRTIM_OUTPUTIDLESTATE_INACTIVE;                             // 空闲状态为无效
    HRTIM_TIM_OutputStructure.FaultState = HRTIM_OUTPUTFAULTSTATE_INACTIVE;                           // 故障状态为无效
    HRTIM_TIM_OutputStructure.ChopperModeEnable = HRTIM_OUTPUTCHOPPERMODE_DISABLED;                   // 禁用斩波模式
    HRTIM_TIM_OutputStructure.BurstModeEntryDelayed = HRTIM_OUTPUTBURSTMODEENTRY_REGULAR;             // 常规突发模式入口
    
    // 配置定时器C、D、E的输出通道1
    HRTIM_WaveformOutputConfig(HRTIM1, HRTIM_TIMERINDEX_TIMER_C, HRTIM_OUTPUT_TC1, &HRTIM_TIM_OutputStructure);
    HRTIM_WaveformOutputConfig(HRTIM1, HRTIM_TIMERINDEX_TIMER_D, HRTIM_OUTPUT_TD1, &HRTIM_TIM_OutputStructure);
    HRTIM_WaveformOutputConfig(HRTIM1, HRTIM_TIMERINDEX_TIMER_E, HRTIM_OUTPUT_TE1, &HRTIM_TIM_OutputStructure);
    // 配置事件参数：
    HRTIM_TIM_EVENT_Structure.FastMode = HRTIM_EVENTFASTMODE_ENABLE;                                 // 使能快速模式
    HRTIM_TIM_EVENT_Structure.Filter = HRTIM_EVENTFILTER_1;                                          // 事件滤波器1
    HRTIM_TIM_EVENT_Structure.Polarity = HRTIM_EVENTPOLARITY_HIGH;                                   // 事件极性高
    HRTIM_TIM_EVENT_Structure.Sensitivity = HRTIM_EVENTSENSITIVITY_LEVEL;                            // 事件灵敏度电平
    HRTIM_TIM_EVENT_Structure.Source = HRTIM_EVENTSRC_2;                                             // 事件源2
    HRTIM_EventConfig(HRTIM1, HRTIM_EVENT_2, &HRTIM_TIM_EVENT_Structure);                            // 配置事件2
    
    // 配置ADC触发参数：
    HRTIM_ADCTrigStructure.Trigger = HRTIM_ADCTRIGGEREVENT13_MASTER_PERIOD | HRTIM_ADCTRIGGEREVENT13_MASTER_CMP1;   // 主周期和主比较器1触发
    HRTIM_ADCTrigStructure.UpdateSource = HRTIM_ADCTRIGGERUPDATE_MASTER;                                            // 主定时器更新源
    HRTIM_ADCTriggerConfig(HRTIM1, HRTIM_ADCTRIGGER_1, &HRTIM_ADCTrigStructure);                                    // 配置ADC触发器1
    
    // 设置比较寄存器值：
    HRTIM1->HRTIM_MASTER.MCMP1R = (HRTIM_PEROID >> 1);                            // 主定时器比较器1值（周期的一半）
    
    // 设置定时器C、D、E的比较寄存器初始值
    HRTIM1->HRTIM_TIMERx[HRTIM_TIMERINDEX_TIMER_C].CMP1xR = 0x04;                 // 定时器C比较器1值
    HRTIM1->HRTIM_TIMERx[HRTIM_TIMERINDEX_TIMER_C].CMP2xR = 0x04;                 // 定时器C比较器2值
    HRTIM1->HRTIM_TIMERx[HRTIM_TIMERINDEX_TIMER_C].CMP3xR = 0x04;                 // 定时器C比较器3值
    HRTIM1->HRTIM_TIMERx[HRTIM_TIMERINDEX_TIMER_C].CMP4xR = 0x04;                 // 定时器C比较器4值
    HRTIM1->HRTIM_TIMERx[HRTIM_TIMERINDEX_TIMER_D].CMP1xR = 0x04;                 // 定时器D比较器1值
    HRTIM1->HRTIM_TIMERx[HRTIM_TIMERINDEX_TIMER_D].CMP2xR = 0x04;                 // 定时器D比较器2值
    HRTIM1->HRTIM_TIMERx[HRTIM_TIMERINDEX_TIMER_D].CMP3xR = 0x04;                 // 定时器D比较器3值
    HRTIM1->HRTIM_TIMERx[HRTIM_TIMERINDEX_TIMER_D].CMP4xR = 0x04;                 // 定时器D比较器4值
    HRTIM1->HRTIM_TIMERx[HRTIM_TIMERINDEX_TIMER_E].CMP1xR = 0x04;                 // 定时器E比较器1值
    HRTIM1->HRTIM_TIMERx[HRTIM_TIMERINDEX_TIMER_E].CMP2xR = 0x04;                 // 定时器E比较器2值
    HRTIM1->HRTIM_TIMERx[HRTIM_TIMERINDEX_TIMER_E].CMP3xR = 0x04;                 // 定时器E比较器3值
    HRTIM1->HRTIM_TIMERx[HRTIM_TIMERINDEX_TIMER_E].CMP4xR = 0x04;                 // 定时器E比较器4值
		
    // 启动主定时器和定时器C、D、E的计数器
    HRTIM_WaveformCounterStart(HRTIM1, HRTIM_TIMERID_MASTER);
    HRTIM_WaveformCounterStart(HRTIM1, HRTIM_TIMERID_TIMER_C);
    HRTIM_WaveformCounterStart(HRTIM1, HRTIM_TIMERID_TIMER_D);
    HRTIM_WaveformCounterStart(HRTIM1, HRTIM_TIMERID_TIMER_E);
    
    // 初始状态下停止所有输出通道
    HRTIM_WaveformOutputStop(HRTIM1, HRTIM_OUTPUT_TC1);  // 停止TC1输出
    HRTIM_WaveformOutputStop(HRTIM1, HRTIM_OUTPUT_TC2);  // 停止TC2输出
    HRTIM_WaveformOutputStop(HRTIM1, HRTIM_OUTPUT_TD1);  // 停止TD1输出
    HRTIM_WaveformOutputStop(HRTIM1, HRTIM_OUTPUT_TD2);  // 停止TD2输出
    HRTIM_WaveformOutputStop(HRTIM1, HRTIM_OUTPUT_TE1);  // 停止TE1输出
    HRTIM_WaveformOutputStop(HRTIM1, HRTIM_OUTPUT_TE2);  // 停止TE2输出
		
		
}
