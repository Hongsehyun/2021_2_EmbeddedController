/**
******************************************************************************
* @author  Hong Se Hyun	
* @brief   Embedded Controller:  TEST1_Q2
* 
******************************************************************************
*/
#include "stm32f411xe.h"

#include "ecRCC.h"
#include "ecGPIO.h"
#include "ecEXTI.h"
#include "ecTIM.h"
#include "ecPWM.h"
#include "ecSysTick.h"

void setup(void);

PWM_t pwm1;
PWM_t pwm2;

void EXTI9_5_IRQHandler(void);

static volatile uint32_t flag   = 0;

int main(void){

  setup();
	
	while(1){
		if(flag==0)  PWM_duty(&pwm2, (float) 0);
		if(flag==1)  PWM_duty(&pwm2, (float) 0.5);
		if(flag==2)  PWM_duty(&pwm2, (float) 1);
		if(flag==3) flag=0;
   }
}


void EXTI9_5_IRQHandler(void) {
   if (is_pending_EXTI(5)) {  
		 
		 flag++;
		 
     clear_pending_EXTI(5);
	}
}


void setup(void){	
	
	//RCC_PLL_init();
	RCC_HSI_init();
	SysTick_init(1);	
	
	// EXTI
	GPIO_init(GPIOB, 5, INPUT);
	EXTI_init(GPIOB, 5, FALLING, 0);

	// PWM
	PWM_init(&pwm1, GPIOA, 6);
	PWM_period_ms(&pwm1, 1000);   // 1sec = 1Hz
  
  PWM_init(&pwm2, GPIOB, 3);
	PWM_period_ms(&pwm2, 1);     // 1msec = 1000Hz
}
