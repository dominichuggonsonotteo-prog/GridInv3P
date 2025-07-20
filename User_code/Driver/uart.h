#ifndef __UART_H__
#define __UART_H__

#include "stm32f30x_conf.h"
#include "inv3p.h"

#define UART_HEAD             0xaaff

#define POWER_ONOFF_FLAG      0x0001      
#define IS_CLOSE_FLAG         0x0002      
#define FAN_STATUS_FLAG       0x0004      
#define PLL_ON_FLAG           0x0008      
#define VOLT_RECTIFY_FLAG     0x0010      
#define RESET_EN_FLAG         0x0020      

#define POWER_ONOFF           ((g_cmdFromPC.pcCmdBit & POWER_ONOFF_FLAG) == POWER_ONOFF_FLAG)
#define IS_INV3P_CLOSE        ((g_cmdFromPC.pcCmdBit & IS_CLOSE_FLAG) == IS_CLOSE_FLAG)
#define FAN_EN                ((g_cmdFromPC.pcCmdBit & FAN_STATUS_FLAG) == FAN_STATUS_FLAG)
#define PLL_EN                ((g_cmdFromPC.pcCmdBit & PLL_ON_FLAG) == PLL_ON_FLAG)
#define IS_VOLT_RECT          ((g_cmdFromPC.pcCmdBit & VOLT_RECTIFY_FLAG) == VOLT_RECTIFY_FLAG)
#define RESET_EN              ((g_cmdFromPC.pcCmdBit & RESET_EN_FLAG) == RESET_EN_FLAG)

#define INV3P_ID_REF          ((g_cmdFromPC.idRef) * (0.01f) - (100.0f))     
#define INV3P_IQ_REF          ((g_cmdFromPC.iqRef) * (0.01f) - (100.0f))     

 
#define MAX_ID_REF_x100       11000        
#define MIN_ID_REF_x100       9000         
#define MAX_IQ_REF_x100       11000        
#define MIN_IQ_REF_x100       9000         

 
typedef struct {
    uint16_t head;             
    uint16_t moduRefRsv;       
    uint16_t freqRefRsv;       
    uint16_t vacRefRmsRsv;     
    uint16_t idRef;            
    uint16_t iqRef;            
    uint16_t vdcRefRsv;        
    uint16_t setRsv1;          
    uint16_t setRsv2;          
    uint16_t pcCmdBit;         
    uint16_t xorVertify;       
} PcCmdConfig;

 
typedef struct {
    uint16_t head;             
    uint16_t inv3pVdc;         
    uint16_t inv3pVacRms;      
    uint16_t inv3pId;          
    uint16_t inv3pIq;          
    uint16_t inv3pFreq;        
    uint16_t inv3pModu;        

    uint16_t inv3pFaultFlag;   
    uint16_t inv3pStatusFlag;  

    uint16_t readIdRef;        
    uint16_t readIqRef;        
    uint16_t readVdcRef;       
    uint16_t readModuRef;      
    uint16_t powerRunTimeSec;  

    uint16_t xorVertify;       
} DataReportConfig;


extern PcCmdConfig g_cmdFromPC;
extern PcCmdConfig g_cmdFromPCTemp;
extern DataReportConfig g_sendToPC;

void UartInit(void);

#endif
