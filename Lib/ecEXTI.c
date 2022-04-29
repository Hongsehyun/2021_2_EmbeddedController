#include "stm32f4xx.h"
#include "stm32f411xe.h"

#include "ecGPIO.h"
#include "ecEXTI.h"
#include "ecTIM.h"


void EXTI_init(GPIO_TypeDef *port, unsigned int pin, unsigned int trig_type, unsigned int priority) {
	// Enable SYSCFG peripheral clock
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
//	SYSCFG->EXTICR[pin>>2] |= port_Flag<<((pin&0x3)<<2);
	// Connect the Corresponding External Line to the GPIO
	if(port == GPIOA) {
		SYSCFG->EXTICR[pin>>2] &= ~(0xF << 4*(pin%4));
    SYSCFG->EXTICR[pin>>2] |= (PA_x << 4*(pin%4));
	}                     
	else if(port == GPIOB){
		SYSCFG->EXTICR[pin>>2] &= ~(0xF << 4*(pin%4));
    SYSCFG->EXTICR[pin>>2] |= (PB_x << 4*(pin%4));
	}                     
	else if(port == GPIOC){
		SYSCFG->EXTICR[pin>>2] &= ~(0xF << 4*(pin%4));
    SYSCFG->EXTICR[pin>>2] |= (PC_x << 4*(pin%4));
	}                     
	else if(port == GPIOD){
		SYSCFG->EXTICR[pin>>2] &= ~(0xF << 4*(pin%4));
    SYSCFG->EXTICR[pin>>2] |= (PD_x << 4*(pin%4));
	}                     
	else if(port == GPIOE){
		SYSCFG->EXTICR[pin>>2] &= ~(0xF << 4*(pin%4));
    SYSCFG->EXTICR[pin>>2] |= (PE_x << 4*(pin%4));
	}
	else if(port == GPIOH) {
		SYSCFG->EXTICR[pin>>2] &= ~(0xF << 4*(pin%4));
    SYSCFG->EXTICR[pin>>2] |= (PH_x << 4*(pin%4));
	}
	
	// Configure the trigger edge :: Falling trigger enable(Button: pull-up)
  if(trig_type == RISE)			    EXTI->RTSR |= 1UL << pin;
  else if(trig_type == FALL)		EXTI->FTSR |= 1UL << pin;
	else if(trig_type == BOTH) {
		 EXTI->RTSR |= 1UL << pin;	
		 EXTI->FTSR |= 1UL << pin;
	}
	
	EXTI->IMR |= 1UL << BUTTON_PIN;

	// Interrupt IRQn, Priority
	// You Should Match Pin Number and EXTIx number
	if(pin==0)      {	NVIC_SetPriority(EXTI0_IRQn, priority);
								    NVIC_EnableIRQ(EXTI0_IRQn);                  }
  else if(pin==1) {	NVIC_SetPriority(EXTI1_IRQn, priority);   
		                NVIC_EnableIRQ(EXTI1_IRQn);                  }  
  else if(pin==2) {	NVIC_SetPriority(EXTI2_IRQn, priority);   
		                NVIC_EnableIRQ(EXTI2_IRQn);                  }    
  else if(pin==3) {	NVIC_SetPriority(EXTI3_IRQn, priority);     
		                NVIC_EnableIRQ(EXTI3_IRQn);                  }  
  else if(pin==4) { NVIC_SetPriority(EXTI4_IRQn, priority);     
		                NVIC_EnableIRQ(EXTI4_IRQn);                  }  
  else if(pin>=5 && pin<=9) 	{  NVIC_SetPriority(EXTI9_5_IRQn, priority);
												  		   NVIC_EnableIRQ(EXTI9_5_IRQn);                }  
	else if(pin>=10 && pin<=15)	{  NVIC_SetPriority(EXTI15_10_IRQn, priority);  
														     NVIC_EnableIRQ(EXTI15_10_IRQn);              }  

	EXTI_enable((uint32_t)pin);
}


// mask in IMR
void EXTI_enable(uint32_t pin){
  EXTI->IMR |= 1UL << pin;
}


// unmask in IMR
void EXTI_disable(uint32_t pin) { 
	EXTI->IMR &= ~(1UL << pin);
}


// Determining whether Pending or not.
uint32_t is_pending_EXTI(uint32_t pin) {
	return ((EXTI->PR & 1UL << pin) == 1UL << pin);
}


// Clear Pending
void clear_pending_EXTI(uint32_t pin) {
	EXTI->PR |= 1<<pin ;
}
