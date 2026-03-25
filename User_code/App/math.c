

#include "math.h"  // 包含数学运算相关的头文件

static const float g_mathSinTable[258] = {
    0.00000000f, 0.00613588f, 0.01227154f, 0.01840673f,
    0.02454123f, 0.03067480f, 0.03680722f, 0.04293826f,
    0.04906767f, 0.05519524f, 0.06132074f, 0.06744392f,
    0.07356456f, 0.07968244f, 0.08579731f, 0.09190896f,
    0.09801714f, 0.10412163f, 0.11022221f, 0.11631863f,
    0.12241068f, 0.12849811f, 0.13458071f, 0.14065824f,
    0.14673047f, 0.15279719f, 0.15885814f, 0.16491312f,
    0.17096189f, 0.17700422f, 0.18303989f, 0.18906866f,
    0.19509032f, 0.20110463f, 0.20711138f, 0.21311032f,
    0.21910124f, 0.22508391f, 0.23105811f, 0.23702361f,
    0.24298018f, 0.24892761f, 0.25486566f, 0.26079412f,
    0.26671276f, 0.27262136f, 0.27851969f, 0.28440754f,
    0.29028468f, 0.29615089f, 0.30200595f, 0.30784964f,
    0.31368174f, 0.31950203f, 0.32531029f, 0.33110631f,
    0.33688985f, 0.34266072f, 0.34841868f, 0.35416353f,
    0.35989504f, 0.36561300f, 0.37131719f, 0.37700741f,
    0.38268343f, 0.38834505f, 0.39399204f, 0.39962420f,
    0.40524131f, 0.41084317f, 0.41642956f, 0.42200027f,
    0.42755509f, 0.43309382f, 0.43861624f, 0.44412214f,
    0.44961133f, 0.45508359f, 0.46053871f, 0.46597650f,
    0.47139674f, 0.47679923f, 0.48218377f, 0.48755016f,
    0.49289819f, 0.49822767f, 0.50353838f, 0.50883014f,
    0.51410274f, 0.51935599f, 0.52458968f, 0.52980362f,
    0.53499762f, 0.54017147f, 0.54532499f, 0.55045797f,
    0.55557023f, 0.56066158f, 0.56573181f, 0.57078075f,
    0.57580819f, 0.58081396f, 0.58579786f, 0.59075970f,
    0.59569930f, 0.60061648f, 0.60551104f, 0.61038281f,
    0.61523159f, 0.62005721f, 0.62485949f, 0.62963824f,
    0.63439328f, 0.63912444f, 0.64383154f, 0.64851440f,
    0.65317284f, 0.65780669f, 0.66241578f, 0.66699992f,
    0.67155895f, 0.67609270f, 0.68060100f, 0.68508367f,
    0.68954054f, 0.69397146f, 0.69837625f, 0.70275474f,
    0.70710678f, 0.71143220f, 0.71573083f, 0.72000251f,
    0.72424708f, 0.72846439f, 0.73265427f, 0.73681657f,
    0.74095113f, 0.74505779f, 0.74913639f, 0.75318680f,
    0.75720885f, 0.76120239f, 0.76516727f, 0.76910334f,
    0.77301045f, 0.77688847f, 0.78073723f, 0.78455660f,
    0.78834643f, 0.79210658f, 0.79583690f, 0.79953727f,
    0.80320753f, 0.80684755f, 0.81045720f, 0.81403633f,
    0.81758481f, 0.82110251f, 0.82458930f, 0.82804505f,
    0.83146961f, 0.83486287f, 0.83822471f, 0.84155498f,
    0.84485357f, 0.84812034f, 0.85135519f, 0.85455799f,
    0.85772861f, 0.86086694f, 0.86397286f, 0.86704625f,
    0.87008699f, 0.87309498f, 0.87607009f, 0.87901223f,
    0.88192126f, 0.88479710f, 0.88763962f, 0.89044872f,
    0.89322430f, 0.89596625f, 0.89867447f, 0.90134885f,
    0.90398929f, 0.90659570f, 0.90916798f, 0.91170603f,
    0.91420976f, 0.91667906f, 0.91911385f, 0.92151404f,
    0.92387953f, 0.92621024f, 0.92850608f, 0.93076696f,
    0.93299280f, 0.93518351f, 0.93733901f, 0.93945922f,
    0.94154407f, 0.94359346f, 0.94560733f, 0.94758559f,
    0.94952818f, 0.95143502f, 0.95330604f, 0.95514117f,
    0.95694034f, 0.95870347f, 0.96043052f, 0.96212140f,
    0.96377607f, 0.96539444f, 0.96697647f, 0.96852209f,
    0.97003125f, 0.97150389f, 0.97293995f, 0.97433938f,
    0.97570213f, 0.97702814f, 0.97831737f, 0.97956977f,
    0.98078528f, 0.98196387f, 0.98310549f, 0.98421009f,
    0.98527764f, 0.98630810f, 0.98730142f, 0.98825757f,
    0.98917651f, 0.99005821f, 0.99090264f, 0.99170975f,
    0.99247953f, 0.99321195f, 0.99390697f, 0.99456457f,
    0.99518473f, 0.99576741f, 0.99631261f, 0.99682030f,
    0.99729046f, 0.99772307f, 0.99811811f, 0.99847558f,
    0.99879546f, 0.99907773f, 0.99932238f, 0.99952942f,
    0.99969882f, 0.99983058f, 0.99992470f, 0.99998118f,
    1.00000000f, 1.00000000f  // 结束点用于插值
};

// sin函数的查表运算：输入参数范围为[0,1)，对应角度为[0,2pi)，输出为Sin值
CCMRAM float Sin(float thetaPu)
{
    uint16_t arraySub;   // 查找表索引
    float quartTheta;    // 四分之一周期内的角度（0-0.25）
    float sign;          // 符号位（1或-1）
    float fractionPu;    // 插值系数（0-1）

    // 初始符号为正
    sign = 1.0f;
   
    // 处理负角度：转换为正角度并记录符号
    if (thetaPu < 0) {
        sign *= -1.0f;
        thetaPu *= -1.0f;
    }
   
    // 提取角度的小数部分（0.0 - 1.0）
    quartTheta = thetaPu - (uint16_t)thetaPu;
    
    // 处理第二象限（0.5-1.0）：正弦值为负
    if (quartTheta >= 0.5f) {
        sign *= -1.0f;
        quartTheta -= 0.5f;
    }
    
    // 处理第四象限（0.25-0.5）：转换为第一象限镜像
    if (quartTheta > 0.25f) {
        quartTheta = 0.5f - quartTheta;
    }
    
    // 转换为查找表索引（0-256对应0-90度）
    fractionPu = quartTheta * 1024.0f;  // 1024 = 256*4（每度4个点）
    arraySub = (uint16_t)fractionPu;
    fractionPu -= arraySub;
    
    // 线性插值计算正弦值并应用符号
    return sign * (g_mathSinTable[arraySub] * (1.0f - fractionPu) 
                 + g_mathSinTable[arraySub + 1] * fractionPu);
}

// cos函数计算
CCMRAM float Cos(float thetaPu)
{
    return Sin(thetaPu + 0.25f);  // cos(x) = sin(x + π/2)
}

// tan函数计算
float Tan(float thetaPu)
{
    return Sin(thetaPu) / Cos(thetaPu);  // tan(x) = sin(x)/cos(x)
}

// cot函数计算
float Cot(float thetaPu)
{
    return Cos(thetaPu) / Tan(thetaPu);  // cot(x) = cos(x)/sin(x)
}

// 快速开根号函数
CCMRAM float Sqrt(float x)
{
    float xhalf = 0.5f * x;
    int i = *(int*)&x;            // 将浮点数解释为整数
    i = 0x5f3759df - (i >> 1);    // 初始近似值
    x = *(float*)&i;              // 将整数解释回浮点数
    x = x * (1.5f - xhalf * x * x); // 牛顿迭代
    return 1.0f / x;              // 返回倒数（算法特性）
}

// abc到αβ的坐标变换
CCMRAM void AbcToAlphaBeta(CordinateTransDef * cordinateTransParam)
{
    // Clark变换公式
    cordinateTransParam->paramAlpha = cordinateTransParam->paramA * 0.6667f 
                                   - cordinateTransParam->paramB * 0.3333f 
                                   - cordinateTransParam->paramC * 0.3333f;
    cordinateTransParam->paramBeta = cordinateTransParam->paramB * 0.5774f 
                                  - cordinateTransParam->paramC * 0.5774f;
    cordinateTransParam->paramZero = (cordinateTransParam->paramA 
                                   + cordinateTransParam->paramB 
                                   + cordinateTransParam->paramC) * 0.3333f;
}

// αβ到abc的坐标变换
CCMRAM void AlphaBetaToAbc(CordinateTransDef * cordinateTransParam)
{
    // 反Clark变换公式
    cordinateTransParam->paramA = cordinateTransParam->paramAlpha 
                               + cordinateTransParam->paramZero * 0.5f;
    cordinateTransParam->paramB = cordinateTransParam->paramAlpha * -0.5f 
                               + cordinateTransParam->paramBeta * 0.866f 
                               + cordinateTransParam->paramZero * 0.5f;
    cordinateTransParam->paramC = cordinateTransParam->paramAlpha * -0.5f 
                               - cordinateTransParam->paramBeta * 0.866f 
                               + cordinateTransParam->paramZero * 0.5f;
}

// dq到αβ的坐标变换
CCMRAM void DqToAlphaBeta(CordinateTransDef * cordinateTransParam)
{
    // 反Park变换公式
    cordinateTransParam->paramAlpha = cordinateTransParam->paramD * cordinateTransParam->cosTheta 
                                   - cordinateTransParam->paramQ * cordinateTransParam->sinTheta;
    cordinateTransParam->paramBeta = cordinateTransParam->paramD * cordinateTransParam->sinTheta 
                                  + cordinateTransParam->paramQ * cordinateTransParam->cosTheta;
}

// αβ到dq的坐标变换
CCMRAM void AlphaBetaToDq(CordinateTransDef * cordinateTransParam)
{
    // Park变换公式
    cordinateTransParam->paramD = cordinateTransParam->paramAlpha * cordinateTransParam->cosTheta 
                               + cordinateTransParam->paramBeta * cordinateTransParam->sinTheta;
    cordinateTransParam->paramQ = cordinateTransParam->paramAlpha * -cordinateTransParam->sinTheta 
                               + cordinateTransParam->paramBeta * cordinateTransParam->cosTheta;
}

// abc到dq的坐标变换
void AbcToDq(CordinateTransDef * cordinateTransParam)
{
    // 组合变换（Clark+Park）
    cordinateTransParam->paramD = cordinateTransParam->paramA * cordinateTransParam->cosTheta * 0.6667f 
                               + cordinateTransParam->paramB * (cordinateTransParam->sinTheta * 0.5774f - cordinateTransParam->cosTheta * 0.3333f)
                               - cordinateTransParam->paramC * (cordinateTransParam->sinTheta * 0.5774f + cordinateTransParam->cosTheta * 0.3333f);
    
    cordinateTransParam->paramQ = cordinateTransParam->paramA * (cordinateTransParam->sinTheta * -0.6667f)
                               + cordinateTransParam->paramB * (cordinateTransParam->sinTheta * 0.3333f + cordinateTransParam->cosTheta * 0.5774f)
                               + cordinateTransParam->paramC * (cordinateTransParam->sinTheta * 0.3333f - cordinateTransParam->cosTheta * 0.5774f);
}

// dq到abc的坐标变换
void DqToAbc(CordinateTransDef * cordinateTransParam)
{
    // 组合变换（反Park+反Clark）
    cordinateTransParam->paramA = cordinateTransParam->paramD * cordinateTransParam->cosTheta 
                               - cordinateTransParam->paramQ * cordinateTransParam->sinTheta;
    
    cordinateTransParam->paramB = cordinateTransParam->paramD * (cordinateTransParam->sinTheta * 0.866f - cordinateTransParam->cosTheta * 0.5f)
                               + cordinateTransParam->paramQ * (cordinateTransParam->sinTheta * 0.5f + cordinateTransParam->cosTheta * 0.866f);
    
    cordinateTransParam->paramC = cordinateTransParam->paramD * (cordinateTransParam->sinTheta * -0.866f - cordinateTransParam->cosTheta * 0.5f)
                               + cordinateTransParam->paramQ * (cordinateTransParam->sinTheta * 0.5f - cordinateTransParam->cosTheta * 0.866f);
}
