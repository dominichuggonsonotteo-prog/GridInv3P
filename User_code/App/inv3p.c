

#include "inv3p.h"
// PLL环路计算变量
PllLoopConfig g_pllLoop = {0};
// Inv3p环路计算变量
Inv3pLoopConfig g_inv3pLoop = {0};
// Inv3p坐标变换变量
CordinateTransDef g_vacReal = {0};
CordinateTransDef g_iacReal = {0};
CordinateTransDef g_iacRef = {0};
CordinateTransDef g_modulateSign = {0};
// 经过零偏矫正后的AC量
float g_vacRealACorrect = 0.0f;
float g_vacRealBCorrect = 0.0f;
float g_vacRealCCorrect = 0.0f;
float g_iacRealACorrect = 0.0f;
float g_iacRealBCorrect = 0.0f;
float g_iacRealCCorrect = 0.0f;
// 主中断计数
uint16_t g_inv3pLoopCnt = 0;

// 三相PLL的PI控制器参数初始化函数
void PllLoopParamInit(void)
{
    g_pllLoop.pllLoop.kp = (22.9f);
    g_pllLoop.pllLoop.ki = (741.6f) * CONTROL_PEROID;
    g_pllLoop.freq = PLL_FREQ_RATED;
}

// Inv3p电压和电流环控制器参数初始化函数
void Inv3pLoopParamInit(void)
{
    g_inv3pLoop.acCurrAlphaLoop.kp = (0.75);
    g_inv3pLoop.acCurrAlphaLoop.kr = (100.0f);
    g_inv3pLoop.acCurrAlphaLoop.ts = CONTROL_PEROID;
    g_inv3pLoop.acCurrAlphaLoop.squreOfw0 = PLL_FREQ_RATED * PLL_FREQ_RATED * PI * PI * (4.0f);
    g_inv3pLoop.acCurrBetaLoop.kp = (0.75);
    g_inv3pLoop.acCurrBetaLoop.kr = (100.0f);
    g_inv3pLoop.acCurrBetaLoop.ts = CONTROL_PEROID;
    g_inv3pLoop.acCurrBetaLoop.squreOfw0 = PLL_FREQ_RATED * PLL_FREQ_RATED * PI * PI * (4.0f);
    // Inv3p三相谐波电流控制器参数初始化
    Inv3pHarmLoopParamInit();
}

// Inv3p三相谐波电流控制器参数初始化
void Inv3pHarmLoopParamInit(void)
{
    g_inv3pLoop.acHarm5CurrAlphaLoop.kr = (20.0f);
    g_inv3pLoop.acHarm5CurrAlphaLoop.ts = CONTROL_PEROID;
    g_inv3pLoop.acHarm5CurrAlphaLoop.squreOfw0 = PLL_FREQ_RATED * PLL_FREQ_RATED * PI * PI * (4.0f) * (25.0f);
    g_inv3pLoop.acHarm5CurrBetaLoop.kr = (20.0f);
    g_inv3pLoop.acHarm5CurrBetaLoop.ts = CONTROL_PEROID;
    g_inv3pLoop.acHarm5CurrBetaLoop.squreOfw0 = PLL_FREQ_RATED * PLL_FREQ_RATED * PI * PI * (4.0f) * (25.0f);
    g_inv3pLoop.acHarm7CurrAlphaLoop.kr = (20.0f);
    g_inv3pLoop.acHarm7CurrAlphaLoop.ts = CONTROL_PEROID;
    g_inv3pLoop.acHarm7CurrAlphaLoop.squreOfw0 = PLL_FREQ_RATED * PLL_FREQ_RATED * PI * PI * (4.0f) * (49.0f);
    g_inv3pLoop.acHarm7CurrBetaLoop.kr = (20.0f);
    g_inv3pLoop.acHarm7CurrBetaLoop.ts = CONTROL_PEROID;
    g_inv3pLoop.acHarm7CurrBetaLoop.squreOfw0 = PLL_FREQ_RATED * PLL_FREQ_RATED * PI * PI * (4.0f) * (49.0f);
}

// PLL闭环控制器复位函数
void PllLoopClear(void)
{
    g_pllLoop.pllLoop.error = (0.0f);
    g_pllLoop.pllLoop.preError = (0.0f);
    g_pllLoop.pllLoop.output = (0.0f);
    g_pllLoop.pllLoop.preOutput = (0.0f);
    g_pllLoop.freq = PLL_FREQ_RATED;
    g_pllLoop.theta = (0.0f);
    g_pllLoop.virtualVectorFilter.alphaIntegrateOut = (0.0f);
    g_pllLoop.virtualVectorFilter.betaIntegrateOut = (0.0f);
    g_pllLoop.virtualVectorFilter.alphaOut = (0.0f);
    g_pllLoop.virtualVectorFilter.betaOut = (0.0f);
}

 
// Inv3p闭环控制器复位函数
void Inv3pLoopClear(void)
{
    // 清零α轴电流控制器状态
    g_inv3pLoop.acCurrAlphaLoop.delay1 = 0.0f;    // 谐振控制器延迟状态1
    g_inv3pLoop.acCurrAlphaLoop.delay2 = 0.0f;    // 谐振控制器延迟状态2
    g_inv3pLoop.acCurrAlphaLoop.krOutput = 0.0f;  // 谐振控制器输出
    g_inv3pLoop.acCurrAlphaLoop.output = 0.0f;    // 总输出

    // 清零β轴电流控制器状态
    g_inv3pLoop.acCurrBetaLoop.delay1 = 0.0f;     // 谐振控制器延迟状态1
    g_inv3pLoop.acCurrBetaLoop.delay2 = 0.0f;     // 谐振控制器延迟状态2
    g_inv3pLoop.acCurrBetaLoop.krOutput = 0.0f;   // 谐振控制器输出
    g_inv3pLoop.acCurrBetaLoop.output = 0.0f;     // 总输出

    // 重置PWM占空比和比较值
    g_inv3pLoop.dutyCycleA = 0.0f;    // A相占空比
    g_inv3pLoop.dutyCycleB = 0.0f;    // B相占空比
    g_inv3pLoop.dutyCycleC = 0.0f;    // C相占空比
    g_inv3pLoop.compareCntA = 0;      // A相比较值
    g_inv3pLoop.compareCntB = 0;      // B相比较值
    g_inv3pLoop.compareCntC = 0;      // C相比较值
}

 
// Inv3p三相谐波电流控制器复位函数
void Inv3pHarmLoopClear(void) 
{
    g_inv3pLoop.acHarm5CurrAlphaLoop.delay1 = (0.0f);
    g_inv3pLoop.acHarm5CurrAlphaLoop.delay2 = (0.0f);
    g_inv3pLoop.acHarm5CurrAlphaLoop.krOutput = (0.0f);
    g_inv3pLoop.acHarm5CurrBetaLoop.delay1 = (0.0f);
    g_inv3pLoop.acHarm5CurrBetaLoop.delay2 = (0.0f);
    g_inv3pLoop.acHarm5CurrBetaLoop.krOutput = (0.0f);
    g_inv3pLoop.acHarm7CurrAlphaLoop.delay1 = (0.0f);
    g_inv3pLoop.acHarm7CurrAlphaLoop.delay2 = (0.0f);
    g_inv3pLoop.acHarm7CurrAlphaLoop.krOutput = (0.0f);
    g_inv3pLoop.acHarm7CurrBetaLoop.delay1 = (0.0f);
    g_inv3pLoop.acHarm7CurrBetaLoop.delay2 = (0.0f);
    g_inv3pLoop.acHarm7CurrBetaLoop.krOutput = (0.0f);
}

// PI控制器运算函数，具有积分抗饱和功能
void PIController(PIControllerDef *controllerOfPI, float outputMax, float outputMin)
{
    // 计算PI控制器输出：位置式算法
    controllerOfPI->output = controllerOfPI->preOutput  // 上一次的输出值作为基础
                           + controllerOfPI->kp * (controllerOfPI->error - controllerOfPI->preError)  // 比例项：当前误差与上次误差的差值
                           + controllerOfPI->ki * controllerOfPI->error;  // 积分项：当前误差的积分
    
    // 输出限幅处理，防止输出超出允许范围
    if (controllerOfPI->output > outputMax) {  // 检查是否超过上限
        controllerOfPI->output = outputMax;    // 钳位到上限值
    } else if (controllerOfPI->output < outputMin) {  // 检查是否低于下限
        controllerOfPI->output = outputMin;    // 钳位到下限值
    }
    
    // 更新状态变量，为下一次计算做准备
    controllerOfPI->preError = controllerOfPI->error;   // 保存当前误差作为下一次的前次误差
    controllerOfPI->preOutput = controllerOfPI->output; // 保存当前输出作为下一次的前次输出
}

 
// PR控制器运算函数
CCMRAM void PRController(PRControllerDef * controllerOfPR)
{
    float sumTemp;  // 用于存储中间计算结果
    // 计算中间状态：二阶广义积分器(SOGI)的离散化实现
    sumTemp = controllerOfPR->squreOfw0 * controllerOfPR->delay1 * controllerOfPR->ts  // w02 * x1 * Ts
            + controllerOfPR->delay2;  // + x2
    
    // 更新状态变量：实现谐振控制器的核心差分方程
    controllerOfPR->delay1 += (controllerOfPR->error * PI_2 * controllerOfPR->kr  // 误差输入项：2π * Kr * e(k)
                           - (controllerOfPR->delay1 * PI_2 + sumTemp)) * controllerOfPR->ts;  // 状态反馈项：(2π * x1 + w02*x1*Ts + x2)
    controllerOfPR->delay2 = sumTemp;  // 更新第二个状态变量
    
    // 计算谐振输出和总输出
    controllerOfPR->krOutput = controllerOfPR->delay1;  // 谐振控制器输出
    controllerOfPR->output = controllerOfPR->krOutput + controllerOfPR->kp * controllerOfPR->error;  // 总输出 = 谐振输出 + 比例项
}

 
// PR控制器运算函数
CCMRAM void HarmonyPRController(HarmonryPRControllerDef * controllerOfPR) 
{
    float sumTemp;
    sumTemp = controllerOfPR->squreOfw0 * controllerOfPR->delay1 * controllerOfPR->ts + controllerOfPR->delay2;
    controllerOfPR->delay1 += (controllerOfPR->error * PI_2 * controllerOfPR->kr - (controllerOfPR->delay1 * PI_2 + sumTemp))
        * controllerOfPR->ts;
    controllerOfPR->delay2 = sumTemp;
    controllerOfPR->krOutput = controllerOfPR->delay1;
}

 
// 频率到角度的积分计算，θ=wt
void PllThetaGenreation(void)
{
    // 更新相位角：θ = θ + f * T (积分过程)
    g_pllLoop.theta += g_pllLoop.freq * CONTROL_PEROID;
    
    // 相位角归一化处理（0~1对应0~2π）
    if (g_pllLoop.theta >= 1.0f) {
        g_pllLoop.theta -= 1.0f;  // 保持相位在[0,1)范围内
    }
}

 
// αβ坐标系下的复矢量滤波器计算函数
void VirtualVectorFilter(VirtualVectorFilterDef * virtualVectorFilterParam, float freq) 
{
    float angleFreq = freq * PI_2;
    virtualVectorFilterParam->alphaIntegrateIn = virtualVectorFilterParam->alphaIn - virtualVectorFilterParam->alphaOut - \
        virtualVectorFilterParam->betaIntegrateOut * angleFreq;
    virtualVectorFilterParam->alphaIntegrateOut += virtualVectorFilterParam->alphaIntegrateIn * CONTROL_PEROID;
    virtualVectorFilterParam->alphaOut = virtualVectorFilterParam->alphaIntegrateOut * PI_10;
    virtualVectorFilterParam->betaIntegrateIn = virtualVectorFilterParam->betaIn - virtualVectorFilterParam->betaOut + \
        virtualVectorFilterParam->alphaIntegrateOut * angleFreq;
    virtualVectorFilterParam->betaIntegrateOut += virtualVectorFilterParam->betaIntegrateIn * CONTROL_PEROID;
    virtualVectorFilterParam->betaOut = virtualVectorFilterParam->betaIntegrateOut * PI_10;
}

 
// 三次谐波注入占空比量计算，用于提高AC电压的调制比
void ThirdHarmonCalca (Inv3pLoopConfig * loopParam) 
{
    float maxDuty;
    float minDuty;
    maxDuty = MAX(loopParam->dutyCycleA, loopParam->dutyCycleB);
    maxDuty = MAX(loopParam->dutyCycleC, maxDuty);
    minDuty = MIN(loopParam->dutyCycleA, loopParam->dutyCycleB);
    minDuty = MIN(loopParam->dutyCycleC, minDuty);
    loopParam->thirdHarmonicInject = (maxDuty + minDuty) * (0.5f);
}

 
// Inv3p坐标变换
void Inv3pCordinateTrans(void) 
{
    // 坐标变换
    g_iacReal.paramA = g_iacRealACorrect;
    g_iacReal.paramB = g_iacRealBCorrect;
    g_iacReal.paramC = g_iacRealCCorrect;
    g_iacReal.sinTheta = g_pllLoop.cordinateTrans.sinTheta;
    g_iacReal.cosTheta = g_pllLoop.cordinateTrans.cosTheta;
    AbcToAlphaBeta(&g_iacReal);
    g_vacReal.paramA = g_vacRealACorrect;
    g_vacReal.paramB = g_vacRealBCorrect;
    g_vacReal.paramC = g_vacRealCCorrect;
    g_vacReal.sinTheta = g_pllLoop.cordinateTrans.sinTheta;
    g_vacReal.cosTheta = g_pllLoop.cordinateTrans.cosTheta;
    AbcToAlphaBeta(&g_vacReal);
}

 
// Inv3p电流内环计算函数
void Inv3pCurrLoop(void) 
{
    // 并网电流指令值赋值
    g_inv3pLoop.idRef = g_inv3pIdRefRamp;
    g_inv3pLoop.iqRef = g_inv3pIqRefRamp;
    // AC电流指令坐标变换到αβ轴
    g_iacRef.paramD = g_inv3pLoop.idRef;
    g_iacRef.paramQ = g_inv3pLoop.iqRef;
    g_iacRef.sinTheta = g_pllLoop.cordinateTrans.sinTheta;
    g_iacRef.cosTheta = g_pllLoop.cordinateTrans.cosTheta;
    DqToAlphaBeta(&g_iacRef);
    // αβ轴下指令值与反馈值赋值
    g_inv3pLoop.ialphaRef = g_iacRef.paramAlpha;
    g_inv3pLoop.ibetaRef = g_iacRef.paramBeta;
    g_inv3pLoop.ialphaReal = g_iacReal.paramAlpha;
    g_inv3pLoop.ibetaReal = g_iacReal.paramBeta;
    g_inv3pLoop.valphaReal = g_vacReal.paramAlpha;
    g_inv3pLoop.vbetaReal = g_vacReal.paramBeta;
    // 电流内环误差值计算
    g_inv3pLoop.acCurrAlphaLoop.error = g_inv3pLoop.ialphaRef - g_inv3pLoop.ialphaReal;
    g_inv3pLoop.acCurrBetaLoop.error = g_inv3pLoop.ibetaRef - g_inv3pLoop.ibetaReal;
    // 电流内环PR控制器计算
    PRController(&g_inv3pLoop.acCurrAlphaLoop);
    PRController(&g_inv3pLoop.acCurrBetaLoop);
    // 对控制器输出的调制信号做输出电压前馈和输入电压前馈
    g_modulateSign.paramAlpha = (g_inv3pLoop.acCurrAlphaLoop.output + g_inv3pLoop.acHarm5CurrAlphaLoop.krOutput + \
        g_inv3pLoop.acHarm7CurrAlphaLoop.krOutput + g_inv3pLoop.valphaReal * (0.8f)) * 2 / g_vdcRealFilter;
    g_modulateSign.paramBeta = (g_inv3pLoop.acCurrBetaLoop.output + g_inv3pLoop.acHarm5CurrBetaLoop.krOutput + \
        g_inv3pLoop.acHarm7CurrBetaLoop.krOutput + g_inv3pLoop.vbetaReal * (0.8f)) * 2 / g_vdcRealFilter;
    // 将调制信号变换到ABC坐标系
    AlphaBetaToAbc(&g_modulateSign);
    g_inv3pLoop.dutyCycleA = g_modulateSign.paramA;
    g_inv3pLoop.dutyCycleB = g_modulateSign.paramB;
    g_inv3pLoop.dutyCycleC = g_modulateSign.paramC;
}

 
// Inv3p 5/7次谐波电流环计算函数
void Inv3pHarm5_7CurrLoop(void) 
{
    // 设置5次谐波控制器误差
    g_inv3pLoop.acHarm5CurrAlphaLoop.error = g_inv3pLoop.acCurrAlphaLoop.error; // α轴5次谐波误差
    g_inv3pLoop.acHarm5CurrBetaLoop.error = g_inv3pLoop.acCurrBetaLoop.error;   // β轴5次谐波误差
    // 执行5次谐波控制
    HarmonyPRController(&g_inv3pLoop.acHarm5CurrAlphaLoop); // α轴5次谐波抑制
    HarmonyPRController(&g_inv3pLoop.acHarm5CurrBetaLoop);  // β轴5次谐波抑制

    // 设置7次谐波控制器误差
    g_inv3pLoop.acHarm7CurrAlphaLoop.error = g_inv3pLoop.acCurrAlphaLoop.error; // α轴7次谐波误差
    g_inv3pLoop.acHarm7CurrBetaLoop.error = g_inv3pLoop.acCurrBetaLoop.error;   // β轴7次谐波误差
    // 执行7次谐波控制
    HarmonyPRController(&g_inv3pLoop.acHarm7CurrAlphaLoop); // α轴7次谐波抑制
    HarmonyPRController(&g_inv3pLoop.acHarm7CurrBetaLoop);  // β轴7次谐波抑制
}

 
// 开启PWM发波函数
void Inv3pPwmOn(void) 
{
    // 启动HRTIM定时器的所有PWM输出通道
    HRTIM_WaveformOutputStart(HRTIM1,HRTIM_OUTPUT_TC1); // TC1通道
    HRTIM_WaveformOutputStart(HRTIM1,HRTIM_OUTPUT_TC2); // TC2通道
    HRTIM_WaveformOutputStart(HRTIM1,HRTIM_OUTPUT_TD1); // TD1通道
    HRTIM_WaveformOutputStart(HRTIM1,HRTIM_OUTPUT_TD2); // TD2通道
    HRTIM_WaveformOutputStart(HRTIM1,HRTIM_OUTPUT_TE1); // TE1通道
    HRTIM_WaveformOutputStart(HRTIM1,HRTIM_OUTPUT_TE2); // TE2通道
}

 
// 关闭PWM发波函数
void Inv3pPwmOff(void) 
{
    // 停止HRTIM定时器的所有PWM输出通道
    HRTIM_WaveformOutputStop(HRTIM1,HRTIM_OUTPUT_TC1); 
    HRTIM_WaveformOutputStop(HRTIM1,HRTIM_OUTPUT_TC2); 
    HRTIM_WaveformOutputStop(HRTIM1,HRTIM_OUTPUT_TD1); 
    HRTIM_WaveformOutputStop(HRTIM1,HRTIM_OUTPUT_TD2); 
    HRTIM_WaveformOutputStop(HRTIM1,HRTIM_OUTPUT_TE1); 
    HRTIM_WaveformOutputStop(HRTIM1,HRTIM_OUTPUT_TE2); 

    // 重置比较寄存器为安全值
    HRTIM1->HRTIM_TIMERx[HRTIM_TIMERINDEX_TIMER_C].CMP1xR = 0x04; // 通道C比较寄存器1
    HRTIM1->HRTIM_TIMERx[HRTIM_TIMERINDEX_TIMER_C].CMP2xR = 0x04; // 通道C比较寄存器2
    HRTIM1->HRTIM_TIMERx[HRTIM_TIMERINDEX_TIMER_C].CMP3xR = 0x04; // 通道C比较寄存器3
    HRTIM1->HRTIM_TIMERx[HRTIM_TIMERINDEX_TIMER_C].CMP4xR = 0x04; // 通道C比较寄存器4
    HRTIM1->HRTIM_TIMERx[HRTIM_TIMERINDEX_TIMER_D].CMP1xR = 0x04; // 通道D比较寄存器1
    HRTIM1->HRTIM_TIMERx[HRTIM_TIMERINDEX_TIMER_D].CMP2xR = 0x04; // 通道D比较寄存器2
    HRTIM1->HRTIM_TIMERx[HRTIM_TIMERINDEX_TIMER_D].CMP3xR = 0x04; // 通道D比较寄存器3
    HRTIM1->HRTIM_TIMERx[HRTIM_TIMERINDEX_TIMER_D].CMP4xR = 0x04; // 通道D比较寄存器4
    HRTIM1->HRTIM_TIMERx[HRTIM_TIMERINDEX_TIMER_E].CMP1xR = 0x04; // 通道E比较寄存器1
    HRTIM1->HRTIM_TIMERx[HRTIM_TIMERINDEX_TIMER_E].CMP2xR = 0x04; // 通道E比较寄存器2
    HRTIM1->HRTIM_TIMERx[HRTIM_TIMERINDEX_TIMER_E].CMP3xR = 0x04; // 通道E比较寄存器3
    HRTIM1->HRTIM_TIMERx[HRTIM_TIMERINDEX_TIMER_E].CMP4xR = 0x04; // 通道E比较寄存器4
}

 
// Inv3p调节PWM发波占空比函数
CCMRAM void OutputOfInv3pPwm(void)
{
    // 调制信号中三次谐波注入，增加AC电压的调制比，与SVPWM等效
    ThirdHarmonCalca(&g_inv3pLoop);
    g_inv3pLoop.dutyCycleA -= g_inv3pLoop.thirdHarmonicInject;
    g_inv3pLoop.dutyCycleB -= g_inv3pLoop.thirdHarmonicInject;
    g_inv3pLoop.dutyCycleC -= g_inv3pLoop.thirdHarmonicInject;
    // 将三个桥臂的占空比标幺至(0~1)
    g_inv3pLoop.dutyCycleA = g_inv3pLoop.dutyCycleA * (0.5f) + (0.5f);
    g_inv3pLoop.dutyCycleB = g_inv3pLoop.dutyCycleB * (0.5f) + (0.5f);
    g_inv3pLoop.dutyCycleC = g_inv3pLoop.dutyCycleC * (0.5f) + (0.5f);
    CLAMP(g_inv3pLoop.dutyCycleA, (1.0f), (0.0f));
    CLAMP(g_inv3pLoop.dutyCycleB, (1.0f), (0.0f));
    CLAMP(g_inv3pLoop.dutyCycleC, (1.0f), (0.0f));
    // 将占空比折算到HRTIMER的比较器值
    g_inv3pLoop.compareCntA = (uint32_t)(g_inv3pLoop.dutyCycleA * HRTIM_PEROID);
    g_inv3pLoop.compareCntB = (uint32_t)(g_inv3pLoop.dutyCycleB * HRTIM_PEROID);
    g_inv3pLoop.compareCntC = (uint32_t)(g_inv3pLoop.dutyCycleC * HRTIM_PEROID);
    // 对计算得到的HRTIMER比较值做最大和最小限幅
    CLAMP(g_inv3pLoop.compareCntA, MAX_COMPARE, MIN_COMPARE);
    CLAMP(g_inv3pLoop.compareCntB, MAX_COMPARE, MIN_COMPARE);
    CLAMP(g_inv3pLoop.compareCntC, MAX_COMPARE, MIN_COMPARE);
    // 更新HRTIMER比较器值
    HRTIM1->HRTIM_TIMERx[HRTIM_TIMERINDEX_TIMER_C].CMP1xR = ((HRTIM_PEROID >> 1) - (g_inv3pLoop.compareCntA >> 1) - DEAD_TIME);
    HRTIM1->HRTIM_TIMERx[HRTIM_TIMERINDEX_TIMER_C].CMP2xR = ((HRTIM_PEROID >> 1) - (g_inv3pLoop.compareCntA >> 1) + DEAD_TIME);
    HRTIM1->HRTIM_TIMERx[HRTIM_TIMERINDEX_TIMER_C].CMP3xR = ((HRTIM_PEROID >> 1) + (g_inv3pLoop.compareCntA >> 1) - DEAD_TIME);
    HRTIM1->HRTIM_TIMERx[HRTIM_TIMERINDEX_TIMER_C].CMP4xR = ((HRTIM_PEROID >> 1) + (g_inv3pLoop.compareCntA >> 1) + DEAD_TIME);
    HRTIM1->HRTIM_TIMERx[HRTIM_TIMERINDEX_TIMER_D].CMP1xR = ((HRTIM_PEROID >> 1) - (g_inv3pLoop.compareCntB >> 1) - DEAD_TIME);
    HRTIM1->HRTIM_TIMERx[HRTIM_TIMERINDEX_TIMER_D].CMP2xR = ((HRTIM_PEROID >> 1) - (g_inv3pLoop.compareCntB >> 1) + DEAD_TIME);
    HRTIM1->HRTIM_TIMERx[HRTIM_TIMERINDEX_TIMER_D].CMP3xR = ((HRTIM_PEROID >> 1) + (g_inv3pLoop.compareCntB >> 1) - DEAD_TIME);
    HRTIM1->HRTIM_TIMERx[HRTIM_TIMERINDEX_TIMER_D].CMP4xR = ((HRTIM_PEROID >> 1) + (g_inv3pLoop.compareCntB >> 1) + DEAD_TIME);
    HRTIM1->HRTIM_TIMERx[HRTIM_TIMERINDEX_TIMER_E].CMP1xR = ((HRTIM_PEROID >> 1) - (g_inv3pLoop.compareCntC >> 1) - DEAD_TIME);
    HRTIM1->HRTIM_TIMERx[HRTIM_TIMERINDEX_TIMER_E].CMP2xR = ((HRTIM_PEROID >> 1) - (g_inv3pLoop.compareCntC >> 1) + DEAD_TIME);
    HRTIM1->HRTIM_TIMERx[HRTIM_TIMERINDEX_TIMER_E].CMP3xR = ((HRTIM_PEROID >> 1) + (g_inv3pLoop.compareCntC >> 1) - DEAD_TIME);
    HRTIM1->HRTIM_TIMERx[HRTIM_TIMERINDEX_TIMER_E].CMP4xR = ((HRTIM_PEROID >> 1) + (g_inv3pLoop.compareCntC >> 1) + DEAD_TIME);
}
 
// PLL锁相的闭环控制
void PllLoopControl(void) 
{
    // 检查锁相环使能状态
    if (g_pllEN == FALSE) { 
        PllLoopClear(); // 禁用时清零PLL状态
        return; 
    }

    // 检查是否启用虚拟矢量滤波
    if (PLL_FILTER_STATUS == TRUE) {  
        // 使用虚拟矢量滤波器
        g_pllLoop.virtualVectorFilter.alphaIn = g_vacReal.paramAlpha; // 设置α轴输入
        g_pllLoop.virtualVectorFilter.betaIn = g_vacReal.paramBeta;   // 设置β轴输入
        VirtualVectorFilter(&g_pllLoop.virtualVectorFilter, PLL_FREQ_RATED); // 执行滤波
        
        // 设置坐标变换参数
        g_pllLoop.cordinateTrans.paramAlpha = g_pllLoop.virtualVectorFilter.alphaOut; // 滤波后α轴分量
        g_pllLoop.cordinateTrans.paramBeta = g_pllLoop.virtualVectorFilter.betaOut;   // 滤波后β轴分量
        g_pllLoop.cordinateTrans.sinTheta = Sin(g_pllLoop.theta);  // 正弦值
        g_pllLoop.cordinateTrans.cosTheta = Cos(g_pllLoop.theta);  // 余弦值
        AlphaBetaToDq(&g_pllLoop.cordinateTrans); // αβ→dq变换
    } else {  
        // 直接使用原始信号
        g_pllLoop.cordinateTrans.paramAlpha = g_vacReal.paramAlpha; // 原始α轴分量
        g_pllLoop.cordinateTrans.paramBeta = g_vacReal.paramBeta;   // 原始β轴分量
        g_pllLoop.cordinateTrans.sinTheta = Sin(g_pllLoop.theta);   // 正弦值
        g_pllLoop.cordinateTrans.cosTheta = Cos(g_pllLoop.theta);   // 余弦值
        AlphaBetaToDq(&g_pllLoop.cordinateTrans); // αβ→dq变换
    }

    // 设置PLL误差信号 (q轴分量)
    g_pllLoop.pllLoop.error = g_pllLoop.cordinateTrans.paramQ; 
    
    // 执行PI控制
    PIController(&g_pllLoop.pllLoop, PLL_MAX_BIAS_FREQ, (-PLL_MAX_BIAS_FREQ)); 
    // 更新频率 = PI输出 + 额定频率
    g_pllLoop.freq = g_pllLoop.pllLoop.output + PLL_FREQ_RATED; 
    
    // 生成新的相位角
    PllThetaGenreation(); 
}

 
// Inv3p控制环路函数
CCMRAM void Inv3pControlLoop(void) 
{
    // 控制循环分频计数器
    static uint16_t divCnt2 = 0; 

    if (++divCnt2 >= 2) { // 每2次循环执行一次
        divCnt2 = 0; // 重置计数器
        Inv3pSampleCalculate(); // 执行采样计算
        Inv3pAcSampleCorrect(); // 执行采样校正
        Inv3pCordinateTrans();  // 执行坐标变换
        
        // 检查逆变器使能状态
        if (IS_INV3P_ON == FALSE) { 
            return; 
        }
        
        // 检查谐波控制使能状态
        if (HARMONY_CURR_STATUS == TRUE) { 
            Inv3pHarm5_7CurrLoop(); // 执行5/7次谐波控制
        }
        
        OutputOfInv3pPwm(); // 更新PWM输出
    } else { 
        Inv3pSampleRaw(); // 原始采样
        PllLoopControl(); // 执行锁相环控制
        
        // 检查逆变器使能状态
        if (IS_INV3P_ON == FALSE) { 
            return; 
        }

        Inv3pCurrLoop(); // 执行电流环控制
    }
    g_inv3pLoopCnt++; // 更新循环计数器
}
