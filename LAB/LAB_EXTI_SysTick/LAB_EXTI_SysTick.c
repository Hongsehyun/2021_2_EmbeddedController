/*
******************************************************************************
* @author  SSSLAB
* @Mod     2021-10-15 by SHHnog
* @brief   Embedded Controller:  Tutorial ___SysTick and EXTI
*
******************************************************************************
*/

//***************************************************************************//
//****************                                          *****************//
//****************   PART B.  LED Toggle with EXTI Button   *****************//
//****************                                          *****************//
//***************************************************************************//
/*
#include "stm32f4xx.h"
#include "stm32f411xe.h"

#include "ecRCC.h"
#include "ecGPIO.h"
#include "ecEXTI.h"
#include "ecSysTick.h"

//#define BUTTON_PIN 13
#define LED_PIN 	 5

void setup(void);
void EXTI15_10_IRQHandler(void);

int main(void) {
  // Initialiization ----------------------------------------------------------
	setup();

	// Infinite Loop ------------------------------------------------------------
	while(1){}
}


void EXTI15_10_IRQHandler(void) {
   if (is_pending_EXTI(BUTTON_PIN)) {
     LED_toggle(GPIOA, LED_PIN);
     clear_pending_EXTI(BUTTON_PIN);
	}
}


void setup(void)
{
	// Program setting
	RCC_PLL_init();                          // System Clock = 84MHz	
	EXTI_init(GPIOC, BUTTON_PIN, FALLING, 0);
	// Input Setting
	GPIO_init(GPIOC, BUTTON_PIN, INPUT );    // calls RCC_GPIOA_enable
	GPIO_pudr(GPIOC, BUTTON_PIN, PD);
	// Output Setting
	GPIO_init(GPIOA, LED_PIN, OUTPUT);
	GPIO_output(GPIOA, LED_PIN, Fast, PushPull , N_PUPD);
}
*/










/*
******************************************************************************
* @author  SSSLAB
* @Mod     2021-10-19 by SHHnog
* @brief   Embedded Controller:  Tutorial ___SysTick and EXTI
*
******************************************************************************
*/
//***************************************************************************//
//*************                                                **************//
//*************   PART C. 7-Segment Display with EXTI Button   **************//
//*************                                                **************//
//***************************************************************************//

#include "stm32f4xx.h"
#include "stm32f411xe.h"

#include "ecRCC.h"
#include "ecGPIO.h"
#include "ecEXTI.h"
#include "ecSysTick.h"

void setup(void);
void EXTI15_10_IRQHandler(void);

static volatile uint32_t sysTick_Interval = 1;	  // Define Interval as 1msec
static volatile uint32_t delay 			      = 1000; // 1000cycle = 1msec*1000 = 1sec.

static volatile uint32_t Flag_Reset = 0 ;
static volatile uint32_t cnt = 0;

int main(void) {
  // Initialiization ------------------------------------------------------------
	setup();
	
	// Infinite Loop --------------------------------------------------------------
	while(1) {
			SysTick_reset();

			sevensegment_decode(cnt % 10);
			delay_ms(delay);
		
			if(Flag_Reset==0) {                // Delay 1000ms = 1sec
				cnt++;
				if(cnt > 9)   cnt = 0;
			} 
			if(Flag_Reset==1) {
				TimeDelay = 1;
				cnt = 0;
			  Flag_Reset=0;
	    }			 
	 }
}



void EXTI15_10_IRQHandler(void) {
	 if (is_pending_EXTI(BUTTON_PIN)) {
			clear_pending_EXTI(BUTTON_PIN);
			Flag_Reset = 1;
			delay_ms(0);
	}  
}


void setup(void)
{
	//In Simul , CLK Disable must
	RCC_PLL_init();                        // CLK Freq. 84Mhz
	//Interrupt Setting  :: Systick Interrupt
	SysTick_init(sysTick_Interval);	                   // Systick Interrupt
	//Interrupt Setting  :: Button Interrupt
	EXTI_init(GPIOC, BUTTON_PIN, FALLING, 0);
	
	//Initialization Output_Segment
	sevensegment_init(LED_PIN1, LED_PIN2, LED_PIN3, LED_PIN4, LED_PIN5, LED_PIN6, LED_PIN7, LED_PIN8);
	GPIO_output(GPIOA , LED_PIN1 , Fast , PushPull , N_PUPD);
	GPIO_output(GPIOA , LED_PIN2 , Fast , PushPull , N_PUPD);
	GPIO_output(GPIOA , LED_PIN3 , Fast , PushPull , N_PUPD);
	GPIO_output(GPIOB , LED_PIN4 , Fast , PushPull , N_PUPD);
	GPIO_output(GPIOC , LED_PIN5 , Fast , PushPull , N_PUPD);
	GPIO_output(GPIOA , LED_PIN6 , Fast , PushPull , N_PUPD);
	GPIO_output(GPIOA , LED_PIN7 , Fast , PushPull , N_PUPD);
	GPIO_output(GPIOB , LED_PIN8 , Fast , PushPull , N_PUPD);

	sevensegment_init(LED_PIN1, LED_PIN2, LED_PIN3, LED_PIN4, LED_PIN5, LED_PIN6, LED_PIN7, LED_PIN8);
}
