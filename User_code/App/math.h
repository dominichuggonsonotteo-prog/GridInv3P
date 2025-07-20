#ifndef __MATH_H__
#define __MATH_H__

#include "stm32f30x_conf.h"
#include "main.h"

 
typedef struct {
     
    float paramA;
    float paramB;
    float paramC;
     
    float paramAlpha;
    float paramBeta;
     
    float paramD;
    float paramQ;
     
    float paramZero;
     
    float sinTheta;
    float cosTheta;
} CordinateTransDef;

 
#define  PI              (3.1416f)
#define  PI_2            (6.2832f)
#define  PI_10           (31.416f)
#define  SQRT2           (1.4142f)
#define  SQRT2x1P15      (1.6261f)
#define  SQRT3           (1.7321f)
#define  SQRT2_RECI      (0.7071f)
#define  SQRT3_RECI      (0.5773f)
#define  SQRT2x2         (2.8284f)
#define  SQRT2x2P15      (3.0406f)
#define  SQRT2x2_RECI    (0.3536f)
#define  SQRT2x1P15_RECI (0.615f)

 
#define MAX(a, b) ((a) > (b) ? (a) : (b))
 
#define MIN(a, b) ((a) < (b) ? (a) : (b))
 
#define ABS(a) ((a) > 0 ? (a) : (-a))
 
#define CLAMP(value, max, min)      \
do {                                \
    if ((value) < (min)) {          \
        (value) = (min);            \
    } else if ((value) > (max)) {   \
        (value) = (max);            \
    }                               \
} while(0)

float Sin(float pu);
float Cos(float pu);
float Tan(float pu);
float Cot(float pu);
float Sqrt(float x);

void AbcToAlphaBeta(CordinateTransDef * cordinateTransParam);
void AlphaBetaToAbc(CordinateTransDef * cordinateTransParam);
void DqToAlphaBeta(CordinateTransDef * cordinateTransParam);
void AlphaBetaToDq(CordinateTransDef * cordinateTransParam);
void AbcToDq(CordinateTransDef * cordinateTransParam);
void DqToAbc(CordinateTransDef * cordinateTransParam);

#endif
