/****************************************************************************************
  * @file    inv3p.c
  * @author  PowerX 功率探索
  * @version V1.0.0
  * @date    01-July-2023
  * @淘宝店铺链接：https://gvmu6dghak5o8m0b3kv7j3j8bx49256.taobao.com/shop/view_shop.htm?spm=a1z0k.7628869.0.0.2a5a37deRiWnIl&shop_id=267446508
  * @LegalDeclaration ：本文档内容难免存在Bug，仅限于交流学习，禁止用于任何的商业用途
  * @Copyright   著作权归 “PowerX 功率探索” 所有
*****************************************************************************************/

#include "inv3p.h"
// PLL环路计算变量
PllLoopConfig g_pllLoop={0};
// Inv3p环路计算变量
Inv3pLoopConfig g_inv3pLoop={0};
// Inv3p坐标变换变量
CordinateTransDef g_vacReal={0};
CordinateTransDef g_iacReal={0};
CordinateTransDef g_iacRef={0};
CordinateTransDef g_modulateSign={0};
// 经过零偏矫正后的AC量
floatg_vacRealACorrect=(0.0f);
floatg_vacRealBCorrect=(0.0f);
floatg_vacRealCCorrect=(0.0f);
floatg_iacRealACorrect=(0.0f);
floatg_iacRealBCorrect=(0.0f);
floatg_iacRealCCorrect=(0.0f);
// 主中断计数
uint16_tg_inv3pLoopCnt=0;

// 三相PLL的PI控制器参数初始化函数
void PllLoopParamInit(void)
{
    // 设置锁相环PI控制器参数
    g_pllLoop.pllLoop.kp = 22.9f;                       // 比例系数
    g_pllLoop.pllLoop.ki = 741.6f * CONTROL_PEROID;     // 积分系数（考虑控制周期）

    // 初始化电网频率为额定值
    g_pllLoop.freq = PLL_FREQ_RATED;                    // 默认电网频率(50Hz/60Hz)
}

 
// Inv3p电压和电流环控制器参数初始化函数
void Inv3pLoopParamInit(void)
{
    // α轴电流控制器参数设置 (PR控制器)
    g_inv3pLoop.acCurrAlphaLoop.kp = 0.75f;                     // 比例系数
    g_inv3pLoop.acCurrAlphaLoop.kr = 100.0f;                    // 谐振系数
    g_inv3pLoop.acCurrAlphaLoop.ts = CONTROL_PEROID;            // 控制周期(秒)
    g_inv3pLoop.acCurrAlphaLoop.squreOfw0 = PLL_FREQ_RATED * PLL_FREQ_RATED * PI * PI * 4.0f; // 基波角频率平方(ω?2)

    // β轴电流控制器参数设置 (PR控制器)
    g_inv3pLoop.acCurrBetaLoop.kp = 0.75f;                      // 比例系数
    g_inv3pLoop.acCurrBetaLoop.kr = 100.0f;                     // 谐振系数
    g_inv3pLoop.acCurrBetaLoop.ts = CONTROL_PEROID;             // 控制周期(秒)
    g_inv3pLoop.acCurrBetaLoop.squreOfw0 = PLL_FREQ_RATED * PLL_FREQ_RATED * PI * PI * 4.0f; // 基波角频率平方(ω?2)

    // 初始化谐波控制器参数
    Inv3pHarmLoopParamInit();  // 调用谐波参数初始化函数
}

 
// Inv3p三相谐波电流控制器参数初始化
void Inv3pHarmLoopParamInit(void)
{
    // 5次谐波α轴控制器参数
    g_inv3pLoop.acHarm5CurrAlphaLoop.kr = 20.0f;                     // 谐振系数
    g_inv3pLoop.acHarm5CurrAlphaLoop.ts = CONTROL_PEROID;             // 控制周期
    // 5次谐波角频率平方(5ω?)2
    g_inv3pLoop.acHarm5CurrAlphaLoop.squreOfw0 = PLL_FREQ_RATED * PLL_FREQ_RATED * PI * PI * 4.0f * 25.0f; 

    // 5次谐波β轴控制器参数
    g_inv3pLoop.acHarm5CurrBetaLoop.kr = 20.0f;                      // 谐振系数
    g_inv3pLoop.acHarm5CurrBetaLoop.ts = CONTROL_PEROID;             // 控制周期
    // 5次谐波角频率平方(5ω?)2
    g_inv3pLoop.acHarm5CurrBetaLoop.squreOfw0 = PLL_FREQ_RATED * PLL_FREQ_RATED * PI * PI * 4.0f * 25.0f; 

    // 7次谐波α轴控制器参数
    g_inv3pLoop.acHarm7CurrAlphaLoop.kr = 20.0f;                     // 谐振系数
    g_inv3pLoop.acHarm7CurrAlphaLoop.ts = CONTROL_PEROID;            // 控制周期
    // 7次谐波角频率平方(7ω?)2
    g_inv3pLoop.acHarm7CurrAlphaLoop.squreOfw0 = PLL_FREQ_RATED * PLL_FREQ_RATED * PI * PI * 4.0f * 49.0f; 

    // 7次谐波β轴控制器参数
    g_inv3pLoop.acHarm7CurrBetaLoop.kr = 20.0f;                      // 谐振系数
    g_inv3pLoop.acHarm7CurrBetaLoop.ts = CONTROL_PEROID;              // 控制周期
    // 7次谐波角频率平方(7ω?)2
    g_inv3pLoop.acHarm7CurrBetaLoop.squreOfw0 = PLL_FREQ_RATED * PLL_FREQ_RATED * PI * PI * 4.0f * 49.0f; 
}

 
// PLL闭环控制器复位函数
void PllLoopClear(void)
{
    // 清零PI控制器状态
    g_pllLoop.pllLoop.error = 0.0f;       // 当前误差
    g_pllLoop.pllLoop.preError = 0.0f;    // 前次误差
    g_pllLoop.pllLoop.output = 0.0f;      // 当前输出
    g_pllLoop.pllLoop.preOutput = 0.0f;   // 前次输出

    // 重置频率和相位角
    g_pllLoop.freq = PLL_FREQ_RATED;      // 恢复默认频率
    g_pllLoop.theta = 0.0f;               // 相位角归零

    // 清零虚拟矢量滤波器状态
    g_pllLoop.virtualVectorFilter.alphaIntegrateOut = 0.0f;  // α轴积分输出
    g_pllLoop.virtualVectorFilter.betaIntegrateOut = 0.0f;   // β轴积分输出
    g_pllLoop.virtualVectorFilter.alphaOut = 0.0f;           // α轴输出
    g_pllLoop.virtualVectorFilter.betaOut = 0.0f;            // β轴输出
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

 
void Inv3pHarmLoopClear(void) 
{
    // 清零5次谐波α轴控制器状态
    g_inv3pLoop.acHarm5CurrAlphaLoop.delay1 = 0.0f; 
    g_inv3pLoop.acHarm5CurrAlphaLoop.delay2 = 0.0f; 
    g_inv3pLoop.acHarm5CurrAlphaLoop.krOutput = 0.0f; 

    // 清零5次谐波β轴控制器状态
    g_inv3pLoop.acHarm5CurrBetaLoop.delay1 = 0.0f; 
    g_inv3pLoop.acHarm5CurrBetaLoop.delay2 = 0.0f; 
    g_inv3pLoop.acHarm5CurrBetaLoop.krOutput = 0.0f; 

    // 清零7次谐波α轴控制器状态
    g_inv3pLoop.acHarm7CurrAlphaLoop.delay1 = 0.0f; 
    g_inv3pLoop.acHarm7CurrAlphaLoop.delay2 = 0.0f; 
    g_inv3pLoop.acHarm7CurrAlphaLoop.krOutput = 0.0f; 

    // 清零7次谐波β轴控制器状态
    g_inv3pLoop.acHarm7CurrBetaLoop.delay1 = 0.0f; 
    g_inv3pLoop.acHarm7CurrBetaLoop.delay2 = 0.0f; 
    g_inv3pLoop.acHarm7CurrBetaLoop.krOutput = 0.0f; 
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
CCMRAM void PRController(PRControllerDef *controllerOfPR)
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

 
CCMRAM void HarmonyPRController(HarmonryPRControllerDef * controllerOfPR) 
{
    float sumTemp; // 中间计算结果
    sumTemp = controllerOfPR->squreOfw0 * controllerOfPR->delay1 * controllerOfPR->ts + controllerOfPR->delay2; 
    controllerOfPR->delay1 += (controllerOfPR->error * PI_2 * controllerOfPR->kr - (controllerOfPR->delay1 * PI_2 + sumTemp)) * controllerOfPR->ts; 
    controllerOfPR->delay2 = sumTemp; 
    controllerOfPR->krOutput = controllerOfPR->delay1; // 谐振控制器输出
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
void VirtualVectorFilter(VirtualVectorFilterDef *virtualVectorFilterParam, float freq)
{
    float angleFreq = freq * PI_2; // 角频率 ω = 2πf

    // α轴滤波器输入计算
    virtualVectorFilterParam->alphaIntegrateIn = virtualVectorFilterParam->alphaIn - virtualVectorFilterParam->alphaOut - 
                                                 virtualVectorFilterParam->betaIntegrateOut * angleFreq; 
    // α轴积分器更新
    virtualVectorFilterParam->alphaIntegrateOut += virtualVectorFilterParam->alphaIntegrateIn * CONTROL_PEROID; 
    // α轴输出计算
    virtualVectorFilterParam->alphaOut = virtualVectorFilterParam->alphaIntegrateOut * PI_10; 

    // β轴滤波器输入计算
    virtualVectorFilterParam->betaIntegrateIn = virtualVectorFilterParam->betaIn - virtualVectorFilterParam->betaOut + 
                                                virtualVectorFilterParam->alphaIntegrateOut * angleFreq; 
    // β轴积分器更新
    virtualVectorFilterParam->betaIntegrateOut += virtualVectorFilterParam->betaIntegrateIn * CONTROL_PEROID; 
    // β轴输出计算
    virtualVectorFilterParam->betaOut = virtualVectorFilterParam->betaIntegrateOut * PI_10; 
}

 
// 三次谐波注入占空比量计算，用于提高AC电压的调制比
void ThirdHarmonCalca(Inv3pLoopConfig *loopParam)
{
    float maxDuty; // 最大占空比
    float minDuty; // 最小占空比

    // 计算三相占空比的最大值
    maxDuty = MAX(loopParam->dutyCycleA, loopParam->dutyCycleB); 
    maxDuty = MAX(loopParam->dutyCycleC, maxDuty); 
    // 计算三相占空比的最小值
    minDuty = MIN(loopParam->dutyCycleA, loopParam->dutyCycleB); 
    minDuty = MIN(loopParam->dutyCycleC, minDuty); 

    // 计算三次谐波注入量（中点偏移）
    loopParam->thirdHarmonicInject = (maxDuty + minDuty) * (0.5f); 
}

 
// Inv3p坐标变换
void Inv3pCordinateTrans(void)
{
    // 设置电流采样校正值
    g_iacReal.paramA = g_iacRealACorrect; // A相电流校正
    g_iacReal.paramB = g_iacRealBCorrect; // B相电流校正
    g_iacReal.paramC = g_iacRealCCorrect; // C相电流校正
    g_iacReal.sinTheta = g_pllLoop.cordinateTrans.sinTheta; // 正弦相位
    g_iacReal.cosTheta = g_pllLoop.cordinateTrans.cosTheta; // 余弦相位
    AbcToAlphaBeta(&g_iacReal); // ABC坐标系→αβ坐标系变换

    // 设置电压采样校正值
    g_vacReal.paramA = g_vacRealACorrect; // A相电压校正
    g_vacReal.paramB = g_vacRealBCorrect; // B相电压校正
    g_vacReal.paramC = g_vacRealCCorrect; // C相电压校正
    g_vacReal.sinTheta = g_pllLoop.cordinateTrans.sinTheta; // 正弦相位
    g_vacReal.cosTheta = g_pllLoop.cordinateTrans.cosTheta; // 余弦相位
    AbcToAlphaBeta(&g_vacReal); // ABC坐标系→αβ坐标系变换
}

 
// Inv3p电流内环计算函数
void Inv3pCurrLoop(void)
{
    // 设置d轴和q轴电流参考值
    g_inv3pLoop.idRef = g_inv3pIdRefRamp; // d轴电流参考
    g_inv3pLoop.iqRef = g_inv3pIqRefRamp; // q轴电流参考
    
    // 设置dq坐标系变换参数
    g_iacRef.paramD = g_inv3pLoop.idRef; // d轴分量
    g_iacRef.paramQ = g_inv3pLoop.iqRef; // q轴分量
    g_iacRef.sinTheta = g_pllLoop.cordinateTrans.sinTheta; // 正弦相位
    g_iacRef.cosTheta = g_pllLoop.cordinateTrans.cosTheta; // 余弦相位
    DqToAlphaBeta(&g_iacRef); // dq坐标系→αβ坐标系变换
    
    // 更新控制环变量
    g_inv3pLoop.ialphaRef = g_iacRef.paramAlpha; // α轴电流参考
    g_inv3pLoop.ibetaRef = g_iacRef.paramBeta;   // β轴电流参考
    g_inv3pLoop.ialphaReal = g_iacReal.paramAlpha; // α轴实际电流
    g_inv3pLoop.ibetaReal = g_iacReal.paramBeta;   // β轴实际电流
    g_inv3pLoop.valphaReal = g_vacReal.paramAlpha; // α轴实际电压
    g_inv3pLoop.vbetaReal = g_vacReal.paramBeta;   // β轴实际电压
    
    // 计算电流误差
    g_inv3pLoop.acCurrAlphaLoop.error = g_inv3pLoop.ialphaRef - g_inv3pLoop.ialphaReal; // α轴电流误差
    g_inv3pLoop.acCurrBetaLoop.error = g_inv3pLoop.ibetaRef - g_inv3pLoop.ibetaReal;    // β轴电流误差
    
    // 执行PR控制
    PRController(&g_inv3pLoop.acCurrAlphaLoop); // α轴电流控制
    PRController(&g_inv3pLoop.acCurrBetaLoop);  // β轴电流控制
    
    // 计算调制信号 (αβ坐标系)
    g_modulateSign.paramAlpha = (g_inv3pLoop.acCurrAlphaLoop.output + g_inv3pLoop.acHarm5CurrAlphaLoop.krOutput + 
                                 g_inv3pLoop.acHarm7CurrAlphaLoop.krOutput + g_inv3pLoop.valphaReal * (0.8f)) * 2 / g_vdcRealFilter; 
    g_modulateSign.paramBeta = (g_inv3pLoop.acCurrBetaLoop.output + g_inv3pLoop.acHarm5CurrBetaLoop.krOutput + 
                                g_inv3pLoop.acHarm7CurrBetaLoop.krOutput + g_inv3pLoop.vbetaReal * (0.8f)) * 2 / g_vdcRealFilter; 
    
    // αβ坐标系→ABC坐标系变换
    AlphaBetaToAbc(&g_modulateSign); 
    // 更新三相占空比
    g_inv3pLoop.dutyCycleA = g_modulateSign.paramA; // A相占空比
    g_inv3pLoop.dutyCycleB = g_modulateSign.paramB; // B相占空比
    g_inv3pLoop.dutyCycleC = g_modulateSign.paramC; // C相占空比
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
    // 计算并注入三次谐波
    ThirdHarmonCalca(&g_inv3pLoop); // 计算三次谐波注入量
    g_inv3pLoop.dutyCycleA -= g_inv3pLoop.thirdHarmonicInject; // A相注入
    g_inv3pLoop.dutyCycleB -= g_inv3pLoop.thirdHarmonicInject; // B相注入
    g_inv3pLoop.dutyCycleC -= g_inv3pLoop.thirdHarmonicInject; // C相注入
    
    // 占空比归一化处理 [0,1]范围
    g_inv3pLoop.dutyCycleA = g_inv3pLoop.dutyCycleA * (0.5f) + (0.5f); 
    g_inv3pLoop.dutyCycleB = g_inv3pLoop.dutyCycleB * (0.5f) + (0.5f); 
    g_inv3pLoop.dutyCycleC = g_inv3pLoop.dutyCycleC * (0.5f) + (0.5f); 
    // 占空比限幅 [0,1]
    CLAMP(g_inv3pLoop.dutyCycleA, (1.0f), (0.0f)); 
    CLAMP(g_inv3pLoop.dutyCycleB, (1.0f), (0.0f)); 
    CLAMP(g_inv3pLoop.dutyCycleC, (1.0f), (0.0f)); 
    
    // 占空比转换为比较计数值
    g_inv3pLoop.compareCntA = (uint32_t)(g_inv3pLoop.dutyCycleA * HRTIM_PEROID); // A相比较值
    g_inv3pLoop.compareCntB = (uint32_t)(g_inv3pLoop.dutyCycleB * HRTIM_PEROID); // B相比较值
    g_inv3pLoop.compareCntC = (uint32_t)(g_inv3pLoop.dutyCycleC * HRTIM_PEROID); // C相比较值
    
    // 比较值限幅 [MIN_COMPARE, MAX_COMPARE]
    CLAMP(g_inv3pLoop.compareCntA, MAX_COMPARE, MIN_COMPARE); 
    CLAMP(g_inv3pLoop.compareCntB, MAX_COMPARE, MIN_COMPARE); 
    CLAMP(g_inv3pLoop.compareCntC, MAX_COMPARE, MIN_COMPARE); 
    
    // 设置HRTIM比较寄存器 (考虑死区时间)
    // A相通道配置
    HRTIM1->HRTIM_TIMERx[HRTIM_TIMERINDEX_TIMER_C].CMP1xR = ((HRTIM_PEROID >> 1) - (g_inv3pLoop.compareCntA >> 1) - DEAD_TIME); 
    HRTIM1->HRTIM_TIMERx[HRTIM_TIMERINDEX_TIMER_C].CMP2xR = ((HRTIM_PEROID >> 1) - (g_inv3pLoop.compareCntA >> 1) + DEAD_TIME); 
    HRTIM1->HRTIM_TIMERx[HRTIM_TIMERINDEX_TIMER_C].CMP3xR = ((HRTIM_PEROID >> 1) + (g_inv3pLoop.compareCntA >> 1) - DEAD_TIME); 
    HRTIM1->HRTIM_TIMERx[HRTIM_TIMERINDEX_TIMER_C].CMP4xR = ((HRTIM_PEROID >> 1) + (g_inv3pLoop.compareCntA >> 1) + DEAD_TIME); 
    // B相通道配置
    HRTIM1->HRTIM_TIMERx[HRTIM_TIMERINDEX_TIMER_D].CMP1xR = ((HRTIM_PEROID >> 1) - (g_inv3pLoop.compareCntB >> 1) - DEAD_TIME); 
    HRTIM1->HRTIM_TIMERx[HRTIM_TIMERINDEX_TIMER_D].CMP2xR = ((HRTIM_PEROID >> 1) - (g_inv3pLoop.compareCntB >> 1) + DEAD_TIME); 
    HRTIM1->HRTIM_TIMERx[HRTIM_TIMERINDEX_TIMER_D].CMP3xR = ((HRTIM_PEROID >> 1) + (g_inv3pLoop.compareCntB >> 1) - DEAD_TIME); 
    HRTIM1->HRTIM_TIMERx[HRTIM_TIMERINDEX_TIMER_D].CMP4xR = ((HRTIM_PEROID >> 1) + (g_inv3pLoop.compareCntB >> 1) + DEAD_TIME); 
    // C相通道配置
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
