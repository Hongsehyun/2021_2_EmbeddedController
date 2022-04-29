/*
******************************************************************************
* @author  SSSLAB
* @Mod     2021-10-21 by SHHnog
* @brief   Embedded Controller:  Tutorial ___SysTick and EXTI
*
******************************************************************************
*/
//***************************************************************************//
//*************                                                **************//
//*************             PART D. Create User API            **************//
//*************                                                **************//
//***************************************************************************//

#include "stm32f4xx.h"
#include "stm32f411xe.h"

#include "ecRCC.h"
#include "ecGPIO.h"
#include "ecEXTI.h"
#include "ecSysTick.h"

#include "EC_API.h"

void setup(void);

static volatile uint32_t delay 		  = 1000; // 1000cycle = 1msec*1000 = 1sec.
static volatile uint32_t cnt        = 0;

// API Section
EC_Ticker tick(1);


int main(void) {
  // Initialiization ------------------------------------------------------------
	setup();
	
	// Infinite Loop --------------------------------------------------------------
	while(1) {
		sevensegment_decode(cnt % 10);
 		tick.delay_msec(delay);     // Delay 1000ms = 1sec
		cnt++;
		
		if(cnt==10) cnt=0;
		tick.reset();
	}			 
}


void setup(void)
{
	//In Simul , CLK Disable must
	RCC_PLL_init();                        // CLK Freq. 84Mh
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
}
