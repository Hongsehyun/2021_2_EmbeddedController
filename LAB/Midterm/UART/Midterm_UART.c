/**
******************************************************************************
* @author  SSSLAB
* @Mod		 2021-10-28 by YKKIM  	
* @brief   Embedded Controller:  Midterm Preparation
* 
******************************************************************************
*/

// CCIF, Multiple controlling
#include "stm32f411xe.h"

#include "ecSysTick.h"
#include "ecEXTI.h"
#include "ecGPIO.h"
#include "ecRCC.h"
#include "ecTIM.h"
#include "ecPWM.h"
#include "ecUART.h"  // should be in the same folder with your HAL lib

void setup(void);
void EXTI15_10_IRQHandler(void);
void TIM2_IRQHandler(void);
volatile uint32_t Flag = 0;

PWM_t pwm;

int main(void){
	
  setup();
	//PWM_duty(&pwm, 0);
	TIM2->CCR1 = (TIM2->ARR)/10;   // 3000
	TIM2->CCR2 = (TIM2->ARR)/5;    // 6000
	TIM2->CCR3 = (TIM2->ARR)/2;    // 15000
	
	
	while(1){
		
				if(Flag == 1) {
				//PWM_duty(&pwm, 0.2);
				GPIO_write(GPIOA, 5, HIGH);
				GPIO_write(GPIOB, 3, LOW);
				GPIO_write(GPIOB, 10, LOW);
				delay_ms(100);
				Flag = 0;
				}
				if(Flag == 2) {
				//PWM_duty(&pwm, 0.5);
				//GPIO_write(GPIOA, 5, LOW);
				//GPIO_write(GPIOB, 3, HIGH);
				//GPIO_write(GPIOB, 10, LOW);
				delay_ms(100);
				Flag = 0;
				}
				if(Flag == 3) {
				//PWM_duty(&pwm, 1);
				//GPIO_write(GPIOA, 5, LOW);
				//GPIO_write(GPIOB, 3, LOW);
				//GPIO_write(GPIOB, 10, HIGH);
				delay_ms(100);
				Flag = 0;
		}
	}
}


void TIM2_IRQHandler(void) {
	if(is_CC1IF(TIM2)) {
		Flag = 1;
		clear_CC1IF(TIM2);
	}
	if(is_CC2IF(TIM2)) {
		Flag = 2;
		clear_CC2IF(TIM2);
	}
	if(is_CC3IF(TIM2)) {
		Flag = 3;
		clear_CC3IF(TIM2);
	}
//	if(is_CC4IF(TIM2)) {
//		Flag = 4;
//		clear_CC4IF(TIM2);
//	}
}


void setup(void){	
	
	RCC_PLL_init(); 
	SysTick_init(1);
	
	//GPIO_init(GPIOC, BUTTON_PIN, INPUT);
	//GPIO_pudr(GPIOC, BUTTON_PIN, PU);
	//EXTI_init(GPIOC, BUTTON_PIN, FALLING, 0);
	
	//GPIO_init(GPIOC, BUTTON_PIN, INPUT);   // calls RCC_GPIOC_enable()
	GPIO_init(GPIOA, 5,  OUTPUT);             // calls RCC_GPIOA_enable()
	GPIO_init(GPIOB, 3,  OUTPUT);      // calls RCC_GPIOA_enable()
	GPIO_init(GPIOB, 10, OUTPUT);      // calls RCC_GPIOA_enable()
	//GPIO_init(GPIOB, LED_PIN4, OUTPUT);      // calls RCC_GPIOB_enable()
	
	GPIO_output(GPIOA, 5, Medium, PushPull , PU);
	GPIO_output(GPIOB, 3, Medium, PushPull , PU);
	GPIO_output(GPIOB, 10, Medium, PushPull , PU);
	//GPIO_output(GPIOB, LED_PIN4, Medium, PushPull , PU);
	
	PWM_init(&pwm, GPIOA, 5);
	PWM_init(&pwm, GPIOB, 3);
	PWM_init(&pwm, GPIOB, 10);
	
	PWM_period_ms(&pwm, 3000);
	
	TIM_INT_init(TIM2, 1);
	TIM_INT_enable(TIM2);
  
	//UART2_init();            //  BAUD:  9600
	
	// GPIO configuration ---------------------------------------------------------------------
	//printf("Setup complete\r\n");
}





/*
// time Measuring
#include "stm32f411xe.h"

#include "ecSysTick.h"
#include "ecEXTI.h"
#include "ecGPIO.h"
#include "ecRCC.h"
#include "ecTIM.h"
#include "ecPWM.h"
#include "ecUART.h"  // should be in the same folder with your HAL lib

void setup(void);
void EXTI15_10_IRQHandler(void);
void TIM2_IRQHandler(void);

static volatile uint32_t EXTI_flag  = 0;
static volatile uint32_t print_Val  = 0;

static volatile uint32_t start      = 0;
static volatile uint32_t end        = 0;
static volatile uint32_t cnt        = 0;


int main(void){
	
  setup();
	
	while(1){
		
		if(EXTI_flag ==1 ) {
			start = cnt;
			EXTI_flag++;
		}
		
		if(EXTI_flag == 3) {
			end = cnt;
			
			EXTI_flag = 0 ;
			cnt = 0;
			
			print_Val	= end - start;
			
			printf("Passing Time: %d (ms)\r\n", print_Val);
			delay_ms(100);	
		}
	}
}

void EXTI15_10_IRQHandler(void) {
   if (is_pending_EXTI(BUTTON_PIN)) {    
		 
		 if(EXTI_flag == 0) EXTI_flag = 1;
		 if(EXTI_flag == 2) EXTI_flag = 3;
		 
     clear_pending_EXTI(BUTTON_PIN);
	}
}

void TIM2_IRQHandler(void) {
	if(is_UIF(TIM2)) {
		cnt++;
		clear_UIF(TIM2);
	}
}

void setup(void){	
	
	RCC_PLL_init(); 
	SysTick_init(1);
	
	GPIO_init(GPIOC, BUTTON_PIN, INPUT);
	GPIO_pudr(GPIOC, BUTTON_PIN, PU);
	EXTI_init(GPIOC, BUTTON_PIN, FALLING, 0);
	 
	TIM_INT_init(TIM2, 1);
	TIM_INT_enable(TIM2);
	
  UART2_init();            //  BAUD:  9600
	
	// GPIO configuration ---------------------------------------------------------------------
	printf("Setup complete\r\n");
}
*/


