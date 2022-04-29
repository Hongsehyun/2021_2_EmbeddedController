/**
******************************************************************************
* @author  SSSLAB
* @Mod		 2021-11-11 by SH Hong
* @brief   Embedded Controller:  Tutorial
******************************************************************************
*/

// ************************** Tutorial Code **************************
// ************************** Tutorial Code **************************
// ************************** Tutorial Code **************************	
#include "stm32f411xe.h"
#include "math.h"

#include "ecADC.h"
#include "ecEXTI.h"
#include "ecGPIO.h"
#include "ecPWM.h"
#include "ecRCC.h"
#include "ecStepper.h"
#include "ecSysTick.h"
#include "ecTIM.h"
#include "ecUART.h"
#include "ecADC.h"


//IR parameter//
static volatile double result_v =0;

void setup(void);
void ADC_IRQHandler(void);


int main(void) { 
	// Initialiization --------------------------------------------------------
	setup();
	
	// Inifinite Loop ----------------------------------------------------------
	while(1);
}


void ADC_IRQHandler(void){
	if( is_ADC_EOC() ){       //after finishing sequence
	  result_v = ADC_read();
		printf("voltage = %.3f\r\n",result_v*3.3/4095);
		delay_ms(500);
 }
}


// Initialiization 
void setup(void)
{	
	RCC_PLL_init();                         // System Clock = 84MHz
	SysTick_init(1);
	
	UART2_init();
	ADC_init(GPIOA,1,SW,2);
	
	ADC_start(ADC1);
}

