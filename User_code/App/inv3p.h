#ifndef __INV3P_H__
#define __INV3P_H__

#include "stm32f30x_conf.h"
#include "sample.h"
#include "math.h"
#include "hrtimer.h"

 
#define PLL_FILTER_STATUS    TRUE
 
#define INV3P_HARMONIC_SUPPRESSION_ENABLE FALSE

#define INV3P_AC_VOLTAGE_FEEDFORWARD_GAIN (0.8f)
 
#define PLL_FREQ_RATED       (50.0f)
 
#define PLL_MAX_BIAS_FREQ    (20.0f)

 
typedef struct {
    float kp;             
    float ki;             
    float error;          
    float preError;       
    float output;         
    float preOutput;      
} PIControllerDef;

 
typedef struct {
    float kp;         
    float kr;         
    float error;      
    float ts;         
    float squreOfw0;  
    float delay1;     
    float delay2;     
    float krOutput;   
    float output;     
} PRControllerDef;

 
typedef struct {
    float kr;         
    float error;      
    float ts;         
    float squreOfw0;  
    float delay1;     
    float delay2;     
    float krOutput;   
} HarmonryPRControllerDef;

 
typedef struct {
    float alphaIn;
    float betaIn;
    float alphaIntegrateIn;
    float betaIntegrateIn;
    float alphaIntegrateOut;
    float betaIntegrateOut;
    float alphaOut;
    float betaOut;
} VirtualVectorFilterDef;

 
typedef struct {
    CordinateTransDef cordinateTrans;
    VirtualVectorFilterDef virtualVectorFilter;   
     
    PIControllerDef pllLoop;                      
    float freq;                                   
    float theta;                                  
} PllLoopConfig;

 
typedef struct {
    float idRef;                   
    float iqRef;                   
    float ialphaRef;               
    float ibetaRef;                
    float vdcRef;                  
		
    float valphaReal;              
    float vbetaReal;               
    float ialphaReal;              
    float ibetaReal;               
    float vdcReal;                 
		
    PIControllerDef dcVoltLoop;        
    PRControllerDef acCurrAlphaLoop;   
    PRControllerDef acCurrBetaLoop;    
    HarmonryPRControllerDef acHarm5CurrAlphaLoop;   
    HarmonryPRControllerDef acHarm5CurrBetaLoop;    
    HarmonryPRControllerDef acHarm7CurrAlphaLoop;   
    HarmonryPRControllerDef acHarm7CurrBetaLoop;    

    float thirdHarmonicInject;     
    float dutyCycleA;              
    float dutyCycleB;              
    float dutyCycleC;              

    uint32_t compareCntA;          
    uint32_t compareCntB;          
    uint32_t compareCntC;          

    uint16_t inv3pFaultFlag;       
    uint16_t inv3pStatusFlag;      
} Inv3pLoopConfig;


typedef struct{
	
	uint16_t int1;
	uint16_t int2;
	float f1;
	float f2;
	
} debug;



extern uint16_t g_inv3pLoopCnt;
extern float g_vacRealACorrect;
extern float g_vacRealBCorrect;
extern float g_vacRealCCorrect;
extern float g_iacRealACorrect;
extern float g_iacRealBCorrect;
extern float g_iacRealCCorrect;
extern PllLoopConfig g_pllLoop;
extern Inv3pLoopConfig g_inv3pLoop;
extern CordinateTransDef g_vacReal;
extern CordinateTransDef g_iacReal;
extern CordinateTransDef g_iacRef;
extern CordinateTransDef g_modulateSign;

void PllLoopParamInit(void);
void Inv3pLoopParamInit(void);
void Inv3pHarmLoopParamInit(void);
void PllLoopClear(void);
void Inv3pLoopClear(void);
void Inv3pHarmLoopClear(void);
void PllThetaGenreation(void);
void PllLoopControl(void);
void PRController(PRControllerDef * controllerOfPR);
void PIController(PIControllerDef *controllerOfPI, float outputMax, float outputMin);
void VirtualVectorFilter(VirtualVectorFilterDef * virtualVectorFilterParam, float freq);
void ThirdHarmonCalca (Inv3pLoopConfig * loopParam);
void Inv3pCordinateTrans(void);
void Inv3pCurrLoop(void);
void Inv3pHarm5_7CurrLoop(void);
void Inv3pPwmOn(void);
void Inv3pPwmOff(void);
void OutputOfInv3pPwm(void);
void Inv3pControlLoop(void);

#endif
