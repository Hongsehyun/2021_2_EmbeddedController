/**
******************************************************************************
* @author  SSSLAB
* @Mod		 2021-10-26 by SHHONG  	
* @brief   Embedded Controller:  Tutorial Stepping Motor
* 
******************************************************************************
*/
//#include "stm32f4xx.h"
#include "stm32f411xe.h"

#include "ecRCC.h"
#include "ecGPIO.h"
#include "ecSysTick.h"
#include "ecEXTI.h"
#include "ecTIM.h"
#include "ecPWM.h"
#include "ecStepper.h"

void setup(void);
void EXTI15_10_IRQHandler(void);

int main(void) { 
	// Initialiization --------------------------------------------------------
	setup();
		
	// FULL-STEP   RPM = 2, Rotate 10 Times
	Stepper_step(2048, 0, FULL);  // (Step : 20480   Direction : 0(++)   Mode : FULL)
	Stepper_step(2048, 1, FULL);  // (Step : 20480   Direction : 1(--)   Mode : FULL)
  
	// HALF-STEP   RPM = 2, Rotate 10 Times	
	//Stepper_step(40960, 0, HALF);  // (Step : 40960   Direction : 0(++)   Mode : HALF)
  //Stepper_step(40960, 1, HALF);  // (Step : 40960   Direction : 1(--)   Mode : HALF)
	
	
	// Inifinite Loop ----------------------------------------------------------
	while(1){;}
}


// Initialiization 
void setup(void)
{	
	RCC_PLL_init();                                        // System Clock = 84MHz
	SysTick_us_init(1);                                    // Systick init

	// EXTI Button Interrupt
	GPIO_init(GPIOC, BUTTON_PIN, INPUT);                   // GPIOC pin13 initialization
	EXTI_init(GPIOC, BUTTON_PIN, FALL, 0);              // External Interrupt Setting

	Stepper_init(GPIOB, 10, GPIOB, 4, GPIOB, 5, GPIOB, 3); // Stepper GPIO pin initialization
	Stepper_setSpeed(8, FULL);                                   // set stepper motor speed

  // Finding Max RPM
	// At FULL Mode :: Over 19, Motor Cannot Rotate Properly
	//Stepper_setSpeed(18, FULL);   // Max RPM to out Stepper Motor
	//Stepper_setSpeed(19, FULL);   // unacceptable RPM
	//Stepper_setSpeed(22, FULL);   // unacceptable RPM
	
	// At HALF Mode :: over 19, Motor Cannot Rotate Properly
	//Stepper_setSpeed(18, HALF);   // Max RPM to out Stepper Motor
	//Stepper_setSpeed(19, HALF);   // unacceptable RPM
	//Stepper_setSpeed(22, HALF);   // unacceptable RPM

}

void EXTI15_10_IRQHandler(void) {  
	if (is_pending_EXTI(BUTTON_PIN)) {
		Stepper_stop();
		clear_pending_EXTI(BUTTON_PIN); // cleared by writing '1'
	}
}

