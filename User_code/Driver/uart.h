#ifndef __UART_H__
#define __UART_H__
#include "stm32f30x_conf.h"
#include "inv3p.h"

#define UART_HEAD 0x000B   //0xaaff

#define POWER_ONOFF_FLAG 0x0001 // 串口下发的功率开机标志位
#define IS_CLOSE_FLAG 0x0002 // 串口下发的闭环模式标志位
#define FAN_STATUS_FLAG 0x0004 // 串口下发的使能风扇标志位
#define PLL_ON_FLAG 0x0008 // 串口下发的使能PLL标志位
#define VOLT_RECTIFY_FLAG 0x0010 // 串口下发的电压整流标志位
#define RESET_EN_FLAG 0x0020 // 串口下发的错误清零标志位

#define POWER_ONOFF ((g_cmdFromPC.pcCmdBit & POWER_ONOFF_FLAG) == POWER_ONOFF_FLAG)
#define IS_INV3P_CLOSE ((g_cmdFromPC.pcCmdBit & IS_CLOSE_FLAG) == IS_CLOSE_FLAG)
#define FAN_EN ((g_cmdFromPC.pcCmdBit & FAN_STATUS_FLAG) == FAN_STATUS_FLAG)
#define PLL_EN ((g_cmdFromPC.pcCmdBit & PLL_ON_FLAG) == PLL_ON_FLAG) //上位机发过来的锁相环使能请求1或者0
#define IS_VOLT_RECT ((g_cmdFromPC.pcCmdBit & VOLT_RECTIFY_FLAG) == VOLT_RECTIFY_FLAG)
#define RESET_EN ((g_cmdFromPC.pcCmdBit & RESET_EN_FLAG) == RESET_EN_FLAG)

#define INV3P_ID_REF ((g_cmdFromPC.idRef) * (0.01f) - (100.0f)) // 接受串口下发的d轴电流指令后*0.01-100
#define INV3P_IQ_REF ((g_cmdFromPC.iqRef) * (0.01f) - (100.0f)) // 接受串口下发的q轴电流指令后*0.01-100

// 串口下发AC电流指令格式为 (Iref + 100) * 100
#define MAX_ID_REF_x100 11000 // 串口下发的d轴电流指令上限幅到10.0A， RMS约为7A
#define MIN_ID_REF_x100 9000 // 串口下发的d轴电流指令下限幅到-10.0A， RMS约为-7A
#define MAX_IQ_REF_x100 11000 // 串口下发的q轴电流指令上限幅到7.07A， RMS约为7A
#define MIN_IQ_REF_x100 9000 // 串口下发的q轴电流指令下限幅到-7.07A， RMS约为-7A

// 串口接收PC下发指令结构体
typedef struct {
uint16_t head; // 通信帧头
uint16_t moduRefRsv; // 调制比指令*100
uint16_t freqRefRsv; // 交流频率指令*100
uint16_t vacRefRmsRsv; // 输出交流电压RMS指令*100
uint16_t idRef; // 交流有功电流指令*100
uint16_t iqRef; // 交流无功电流指令*100
uint16_t vdcRefRsv; // 直流电压指令*100
uint16_t setRsv1; // 预留字
uint16_t setRsv2; // 预留字
uint16_t pcCmdBit; // 电源工作模式控制字
uint16_t xorVertify; // 异或校验
} PcCmdConfig;

// 串口上报电源状态结构体
typedef struct {
uint16_t head; // 通信帧头
uint16_t inv3pVdc; // 实际直流电压*100
uint16_t inv3pVacRms; // 实际交流电压RMS*100
uint16_t inv3pId; // 实际交流有功电流*100
uint16_t inv3pIq; // 实际交流无功电流*100
uint16_t inv3pFreq; // 实际交流频率*100
uint16_t inv3pModu; // 实际调制比*100
uint16_t inv3pFaultFlag; // 电源故障标志位
uint16_t inv3pStatusFlag; // 电源运行状态标志位
uint16_t readIdRef; // d轴电流指令值回读*100
uint16_t readIqRef; // q轴电流指令值回读*100
uint16_t readVdcRef; // 直流电压指令值回读*100
uint16_t readModuRef; // 调制比指令值回读*100
uint16_t powerRunTimeSec; // 功率连续运行时间
uint16_t xorVertify; // 异或校验
} DataReportConfig;

extern PcCmdConfig g_cmdFromPC;
extern PcCmdConfig g_cmdFromPCTemp;
extern DataReportConfig g_sendToPC;
void UartInit(void);
#endif


