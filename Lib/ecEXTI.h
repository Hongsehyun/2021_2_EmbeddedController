#ifndef __EC_EXTI_H
#define __EC_EXTI_H

#include "stm32f4xx.h"
#include "stm32f411xe.h"

#include "ecGPIO.h"
#include "ecRCC.h"
#include "ecTIM.h"
#include "ecSysTick.h"

//#define RISING	0
//#define FALLING	1
//#define BOTH	  2

#define PA_x	  0
#define PB_x	  1
#define PC_x	  2 
#define PD_x	  3
#define PE_x  	4
#define PH_x    7

// The three lines below allow the 'C' language to be recognized in 'C++'.
#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */
// The three lines above allow the 'C' language to be recognized in 'C++'.
	 
 void EXTI_init(GPIO_TypeDef *port, unsigned int pin, unsigned int trig_type, unsigned int priority);

 void EXTI_enable(uint32_t pin); // mask in IMR
 
 void EXTI_disable(uint32_t pin); // unmask in IMR
 
 uint32_t is_pending_EXTI(uint32_t pin);

 void clear_pending_EXTI(uint32_t pin);

// The three lines below allow the 'C' language to be recognized in 'C++'.
#ifdef __cplusplus
}
#endif /* __cplusplus */
// The three lines above allow the 'C' language to be recognized in 'C++'.
#endif
