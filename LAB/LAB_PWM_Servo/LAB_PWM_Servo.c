/**
******************************************************************************
* @author  SSSLAB
* @Mod		 2021-10-28 by SH Hong	
* @brief   Embedded Controller:  LAB5. PWM Servo Motor
* 
******************************************************************************
*/
//***************************************************************************//
//****************                                          *****************//
//****************        A. Create EC_HAL Functions        *****************//
//****************              Tutorial Test               *****************//
//****************                                          *****************//
//***************************************************************************//
/*
// Tutorial(1)
#include "stm32f411xe.h"

#include "ecGPIO.h"
#include "ecRCC.h"
#include "ecTIM.h"

#define LED_PIN 	 5

static volatile uint32_t cnt = 0;

void setup(void);
void TIM2_IRQHandler(void);

int main(void) { 
	// Initialiization --------------------------------------------------------
  setup();
	
	// Inifinite Loop ----------------------------------------------------------
	while(1){}
}

void setup(void) {
	RCC_PLL_init();
	GPIO_init(GPIOA, LED_PIN, OUTPUT);
	TIM_INT_init(TIM2, 1);
}

void TIM2_IRQHandler(void) {
	if(is_UIF(TIM2)) {
		cnt++;
		if(cnt>1000) {
			LED_toggle(GPIOA, LED_PIN);
			cnt = 0;
		}
		clear_UIF(TIM2);
	}
}
*/






// Tutorial(2)
#include "stm32f411xe.h"

#include "ecSysTick.h"
#include "ecEXTI.h"
#include "ecGPIO.h"
#include "ecRCC.h"
#include "ecTIM.h"
#include "ecPWM.h"

PWM_t pwm;

void setup(void);

int main(void) { 
	// Initialiization --------------------------------------------------------
  setup();

	// Inifinite Loop ----------------------------------------------------------
   while(1){
		 for(int i=0; i<11; i++){
      PWM_duty(&pwm, 0.1*i);
      delay_ms(100);
		}
			for(int i=10; i>=0; i--){
      PWM_duty(&pwm, 0.1*i);
      delay_ms(100);
		}
	}
}

void setup(void) {	
	//CLK Setting
	RCC_PLL_init();                      // System Clock = 84MHz
	SysTick_init(1);                     // for delay_ms()
	
	//GPIO Setting
	GPIO_init(GPIOA, LED_PIN, AF);       // GPIOA 5 ALTERNATE function
	GPIO_ospeed(GPIOA, LED_PIN, High);   // GPIOA 5 HIGH SPEED

  //PWM Setting
	PWM_init(&pwm, GPIOA, 5);
	PWM_period_us(&pwm, 1000);
}





/**
******************************************************************************
* @author  SSSLAB
* @Mod		 2021-10-26 by SH Hong	
* @brief   Embedded Controller:  LAB5. PWM Servo Motor
* 
******************************************************************************
*/
//***************************************************************************//
//****************                                          *****************//
//****************           LAB5. PWM Servo Motor          *****************//
//****************                                          *****************//
//****************                                          *****************//
//***************************************************************************//
// LAB
/*
#include "stm32f411xe.h"

#include "ecSysTick.h"
#include "ecEXTI.h"
#include "ecGPIO.h"
#include "ecRCC.h"
#include "ecTIM.h"
#include "ecPWM.h"

PWM_t pwm;

void setup(void);
void EXTI15_10_IRQHandler(void);

static volatile uint32_t cnt = 0;
static volatile uint32_t PWM_period = 20;     // msec

static volatile uint32_t Rotation_Flag = 0;
static volatile float cycle= 0;

int main(void) { 
	// Initialiization --------------------------------------------------------
  setup();

	// Inifinite Loop ----------------------------------------------------------
	while(1){
		    if(Rotation_Flag == 1) {
         cycle = (float) ( (float)(5+2*cnt) / 200 );
			   
				 PWM_duty(&pwm,cycle);
         delay_ms(50);
         
				 cnt++;
         Rotation_Flag = 0;
         
         if(cnt>10) {
            delay_ms(1000);
					  cnt =0;
            PWM_duty(&pwm,0.5/20);
      }
		}
	}
}


void EXTI15_10_IRQHandler(void) {
   if (is_pending_EXTI(BUTTON_PIN)) {     
		 Rotation_Flag = 1;
     clear_pending_EXTI(BUTTON_PIN);
	}
}


void setup(void) {	
	//CLK Setting
	RCC_PLL_init();     // System Clock = 84MHz
	SysTick_init(1);    // msec
	
	//Input Setting
	GPIO_init(GPIOC, BUTTON_PIN, INPUT);
	GPIO_pudr(GPIOC, BUTTON_PIN, PU);
	EXTI_init(GPIOC, BUTTON_PIN, FALLING, 0);
	
  //Timer Setting , PWM Setting
	PWM_init(&pwm, GPIOA, PWM_Pin);
	PWM_period_ms(&pwm, PWM_period);
}
*/

/*
//  Another Main Code of This Lab.

		switch(cnt){
      case 0: PWM_duty(&pwm,0.5/20); delay_ms(50); break;
      case 1: PWM_duty(&pwm,0.7/20); delay_ms(50); break;
      case 2: PWM_duty(&pwm,0.9/20); delay_ms(50); break;
      case 3: PWM_duty(&pwm,1.1/20); delay_ms(50); break;
      case 4: PWM_duty(&pwm,1.3/20); delay_ms(50); break;
      case 5: PWM_duty(&pwm,1.5/20); delay_ms(50); break;
      case 6: PWM_duty(&pwm,1.7/20); delay_ms(50); break;
      case 7: PWM_duty(&pwm,1.9/20); delay_ms(50); break;
      case 8: PWM_duty(&pwm,2.1/20); delay_ms(50); break;
      case 9: PWM_duty(&pwm,2.3/20); delay_ms(50); break;
      case 10: PWM_duty(&pwm,2.5/20); delay_ms(1000); cnt =0; break;
			}
*/
