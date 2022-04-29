#ifndef __EC_RCC_H
#define __EC_RCC_H

#include "stm32f4xx.h"
#include "stm32f411xe.h"

#include "ecGPIO.h"
#include "ecSysTick.h"
#include "ecEXTI.h"

// The three lines below allow the 'C' language to be recognized in 'C++'.
#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */
// The three lines above allow the 'C' language to be recognized in 'C++'.


// Part 1. Create EC_HAL Driver
void RCC_HSI_init(void);
void RCC_PLL_init(void);

void RCC_GPIOA_enable(void);
void RCC_GPIOB_enable(void);
void RCC_GPIOC_enable(void);

extern int EC_SYSCL;

// The three lines below allow the 'C' language to be recognized in 'C++'.
#ifdef __cplusplus
}
#endif /* __cplusplus */
// The three lines above allow the 'C' language to be recognized in 'C++'.

#endif
