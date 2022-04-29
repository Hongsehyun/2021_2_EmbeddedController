/**
******************************************************************************
* @author  Hong Se Hyun
* @brief   Embedded Controller:  TEST1_Q1
* 
******************************************************************************
*/

#include "stm32f411xe.h"

#include "ecRCC.h"
#include "ecGPIO.h"
#include "ecEXTI.h"
#include "ecTIM.h"
#include "ecSysTick.h"
#include "ecPWM.h"

void setup(void);
void TIM2_IRQHandler(void);
void EXTI15_10_IRQHandler(void);

static volatile uint32_t cnt   = 0;
static volatile uint32_t flag  = 0;
static volatile uint32_t led_2 = 0;


int main(void){

  setup();

	while(1){
		
		if(flag == 0){
			delay_ms(100);	
			GPIO_write(GPIOA, 5, 0);
			GPIO_write(GPIOB, 6, 0);
		}
		
		if(flag == 1){	
			LED_toggle(GPIOA, 5);
			GPIO_write(GPIOB, 6, 0);
			delay_ms(500);
		}
		
		if(flag == 2 && led_2 == 1) {
			GPIO_write(GPIOA, 5, 0);
			LED_toggle(GPIOB, 6);
			cnt = 0;
			led_2 = 0;
    }
	}
}
	

void TIM2_IRQHandler(void) {
	if(is_UIF(TIM2)) {
		cnt++;
		if(cnt>500) {
			led_2 = 1;
		}
		clear_UIF(TIM2);
	}
}


void EXTI15_10_IRQHandler(void) {
   if (is_pending_EXTI(BUTTON_PIN)) {
		 flag ++;
		 if(flag>2) flag=0;
		 clear_pending_EXTI(BUTTON_PIN);
	}
}



void setup(void){	
	
	RCC_PLL_init();
	SysTick_init(1);
	
	// Input Setting
	GPIO_init(GPIOC, 13, INPUT);	
	GPIO_pudr(GPIOC, 13, PD);
	
	// Output Setting
	GPIO_init(GPIOA, 5, OUTPUT);
	GPIO_init(GPIOB, 6, OUTPUT);
	GPIO_output(GPIOA, 5, Medium, PushPull , N_PUPD);
	GPIO_output(GPIOB, 6, Medium, PushPull , N_PUPD);
	
	// EXTI
	EXTI_init(GPIOC, 13, RISING, 0);
	
	TIM_INT_init(TIM2, 1);  // 1000Hz = 1ms
	TIM_INT_enable(TIM2);
}


