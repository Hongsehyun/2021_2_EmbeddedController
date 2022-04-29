#ifndef __EC_SYSTICK_H
#define __EC_SYSTICK_H

#include "stm32f4xx.h"
#include "stm32f411xe.h"

#include "ecGPIO.h"
#include "ecRCC.h"
#include "ecEXTI.h"

#define MCU_CLK_PLL 84000000
#define MCU_CLK_HSI 16000000

volatile static uint32_t TimeDelay;

// The three lines below allow the 'C' language to be recognized in 'C++'.
#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */
// The three lines above allow the 'C' language to be recognized in 'C++'.
	 
void SysTick_init(uint32_t msec);

void SysTick_us_init(uint32_t usec);
	 
uint32_t SysTick_val(void);

void SysTick_reset(void);

void SysTick_enable(void);

void SysTick_disable(void);

void SysTick_Handler(void);

void delay_ms(uint32_t msec);

void delay_us(uint32_t usec);

// The three lines below allow the 'C' language to be recognized in 'C++'.
#ifdef __cplusplus
}
#endif /* __cplusplus */
// The three lines above allow the 'C' language to be recognized in 'C++'.
#endif
