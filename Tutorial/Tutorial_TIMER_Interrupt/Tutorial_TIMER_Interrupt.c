/**
******************************************************************************
* @author  SSSLAB
* @Mod		 2021-10-22 by YKKIM  	
* @brief   Embedded Controller:  Tutorial Timer
* 
******************************************************************************
*/
//***************************************************************************//
//****************                                          *****************//
//****************     Tutorial Part1. Timer Interrupt      *****************//
//****************                                          *****************//
//***************************************************************************//
/*
#include "stm32f411xe.h"
#include "ecGPIO.h"
#include "ecRCC.h"
//#include "ecTIM.h"

uint32_t cnt=0;

#define LED_PIN 	5

void setup(void);
void TIM2_IRQHandler(void);

int main(void) { 
	// Initialiization --------------------------------------------------------
	// PLL Initialization
	setup();
	
	// Select Timer
	TIM_TypeDef* timerx;
	timerx = TIM2;
	
	// Enable Timer Peripheral Clock: TIM2 
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	
	// TIM2 counting direction: DIR0 
	timerx->PSC = 83;			                 // Timer counter clock Freq: 1MHz(period = 1us)
	timerx->ARR = 999;		                 // PWM motor Freq: 1KHz(period = 1ms)
          // Set auto reload register to maximum (can count up to 65535)
	        // (2^16-1)=65535 = Reload Range
	timerx->DIER |= TIM_DIER_UIE;          // Enable Interrupt
	timerx->CR1  |= TIM_CR1_CEN;           // Enable counter
	
	 NVIC_EnableIRQ(TIM2_IRQn);            // TIM2_IRQHandler Enable
   NVIC_SetPriority(TIM2_IRQn ,2);       // TIM2_IRQHandler Set priority as 2
	 
	// Inifinite Loop ----------------------------------------------------------
	while(1){}
}


void TIM2_IRQHandler(void){
	if((TIM2->SR & TIM_SR_UIF) == TIM_SR_UIF){     // update interrupt flag
		cnt++;
		if (cnt >1000) {
			LED_toggle(GPIOA,LED_PIN);
		  cnt=0;
		}
		TIM2->SR &= ~TIM_SR_UIF;                     // clear by writing 0
		}
}


void setup(void)
{	
	RCC_PLL_init();                       // System Clock = 84MHz
	GPIO_init(GPIOA, LED_PIN, OUTPUT);    // calls RCC_GPIOA_enable()	
}
*/










/*
******************************************************************************
* @author  SSSLAB
* @Mod		 2021-10-22 by YKKIM  	
* @brief   Embedded Controller:  Tutorial PWM
* 
******************************************************************************
*/
//***************************************************************************//
//****************                                          *****************//
//****************        Tutorial Part2. TIMER_PWM         *****************//
//****************                                          *****************//
//***************************************************************************//

#include "stm32f411xe.h"
#include "ecGPIO.h"
#include "ecRCC.h"
#include "ecSysTick.h"
#include "ecEXTI.h"
//#include "ecPWM.h"

#define LED_PIN   	5
#define BUTTON_PIN  13

uint32_t cnt = 0;

void setup(void);

int main(void) { 
	// Initialiization --------------------------------------------------------
  setup();
	
	// TEMP: TIMER Register Initialiization --------------------------------------------------------		
	TIM_TypeDef *TIMx;
	TIMx = TIM2;
	
	// GPIO: ALTERNATIVE function setting
	GPIOA->AFR[0]	 = 1<<(4*LED_PIN);                    // AF1 at PA5 = TIM2_CH1 (p.150)
	
	// TIMER: PWM setting
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;                 // Enable TIMER clock
	
	TIMx->CR1 &= ~TIM_CR1_DIR;                          // Direction Up-count
	
	uint32_t prescaler = 83;
	TIMx->PSC = prescaler;                              // Set Timer CLK: (PSC+1)= 84Mhz/0.1MHz --> PSC= 840-1
	
	TIMx->ARR = 999;                                     // Set ARR:  (ARR+1) = 100kHz/1kHz  --> ARR=100-1.    
	
	TIMx->CCMR1 &= ~TIM_CCMR1_OC1M;                     // Clear ouput compare mode bits for channel 1
	TIMx->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2; // OC1M = 110 for PWM Mode 1 output on ch1. #define TIM_CCMR1_OC1M_1          (0x2UL << TIM_CCMR1_OC1M_Pos)
	TIMx->CCMR1	|= TIM_CCMR1_OC1PE;                     // Output 1 preload enable (make CCR1 value changable)
			
	TIMx->CCER &= ~TIM_CCER_CC1P; // ~(1<<1)            // select output polarity: active high	
	TIMx->CCER  |= TIM_CCER_CC1E;                       // Enable output for ch1
	TIMx->CCR1  = 999/2;                                 // Output Compare Register for channel 1 (default duty ratio = 50%)
	
	TIMx->CR1  |= TIM_CR1_CEN;                          // Enable counter
	
	// Inifinite Loop ----------------------------------------------------------
	while(1){
		for (int i=0;i<10;i++){
		TIM2->CCR1 = 999*i/9;
		delay_ms(100);
		}
	}
}


void setup(void)
{	
	RCC_PLL_init();                      // System Clock = 84MHz
	SysTick_init(1);                     // for delay_ms()
	GPIO_init(GPIOA, LED_PIN, AF);       // GPIOA 5 ALTERNATE function
	GPIO_ospeed(GPIOA, LED_PIN, High);   // GPIOA 5 HIGH SPEED
}

