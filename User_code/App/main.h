#ifndef __MAIN_H__
#define __MAIN_H__

#define NULL  (0)
#define FALSE (0)
#define TRUE  (1)

#define CCMRAM  __attribute__((section("ccmram")))

typedef void (* FunctionPointer)(void);

#endif
