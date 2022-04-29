/**
******************************************************************************
* @author  SSSLAB
* @Mod		 2021-11-09 by SH HONG	
* @brief   Embedded Controller:  UART
******************************************************************************
*/

#include "stm32f411xe.h"
#include "math.h"

#include "ecEXTI.h"
#include "ecGPIO.h"
#include "ecPWM.h"
#include "ecRCC.h"
#include "ecStepper.h"
#include "ecSysTick.h"
#include "ecTIM.h"
#include "ecUART.h"

static volatile uint32_t capture_start = 0;
static volatile uint32_t printFlag     = 0;

static volatile uint32_t ovf_cnt       = 0;
static volatile double distance        = 0;

static volatile float timeInterval     = 0;
static volatile float timeSt           = 0;
static volatile float timeEnd          = 0;
																		   
static volatile float PulseWidth       = 10;  // 10us

void setup(void);
void TIM2_IRQHandler(void);

int main(void){
	
	setup();
	
	// Setting Time = about 6~7 sec.
	printf("Getting ready to run. Please wait a moment.\r\n"); delay_ms(2222);
	printf("Getting ready to run. Please wait a moment.\r\n"); delay_ms(2222);
	printf("Getting ready to run. Please wait a moment.\r\n"); delay_ms(2222);
	
	
	while(1){
		//distance = (float) timeInterval/58;   // Ultrasonic speed[m/s] * echo pulse duration[us]

		if(printFlag==1) {

			if(distance>2 && distance<400) {      // Measuareable
				printf("%.2f [cm] \r\n",distance);    
				delay_ms(500);
			}
			
			else {
				printf("\r");                       // out of range OR Trash Value 
			  delay_us(10);
			}			
			
			printFlag = 0;
		}
	}
}


void TIM2_IRQHandler(void){
	if(is_UIF(TIM2)){                     // Update interrupt
		if(capture_start==1) ovf_cnt++;    	// overflow count
		clear_UIF(TIM2);  							    // clear update interrupt flag
	}
	if(is_CCIF(TIM2,3)){ 									// TIM2_Ch3 (IC3) Capture Flag. Rising Edge Detect
		capture_start = 1;
		timeSt = TIM2->CCR3;				      	// Capture TimeStart from CC3
		clear_CCIF(TIM2,3);                 // clear capture/compare interrupt flag 
	}								                      
	else if(is_CCIF(TIM2,4)){ 				  	// TIM2_Ch4 (IC4) Capture Flag. Falling Edge Detect
		capture_start = 0;
		
		timeEnd = TIM2->CCR4;	      				// Capture TimeEnd from CC4
    clear_CCIF(TIM2,4);								  // clear capture/compare interrupt flag 
		
		//if(timeSt<=timeEnd)  timeInterval = PulseWidth*((timeEnd-timeSt)+(0XFFFF*ovf_cnt));
		//if(timeSt>timeEnd)   timeInterval = PulseWidth*((((TIM2->ARR+1)-timeSt)+timeEnd)+((TIM2->ARR+1)*(ovf_cnt-1)));
		timeInterval = PulseWidth*((timeEnd-timeSt)+(0XFFFF*ovf_cnt));
		distance     = (float) timeInterval/58;   // Ultrasonic speed[m/s] * echo pulse duration[us]
		
		ovf_cnt       = 0;                        // overflow reset
		printFlag     = 1;
	}
}


void setup(){

	RCC_PLL_init(); 
	SysTick_init(1);
	UART2_init();
  
// PWM configuration ---------------------------------------------------------------------	
	PWM_t trig;											// PWM1 for trig
	PWM_init(&trig, GPIOA, 6);		  // PWM init as PA_6: Ultrasonic trig pulse
	PWM_period_us(&trig,50000);    	// PWM of 50ms period. Use period_us()
	PWM_pulsewidth_us(&trig,PulseWidth);   	// PWM pulse width of 10us
	
// Input Capture configuration -----------------------------------------------------------------------	
	IC_t echo;											// Input Capture for echo
	ICAP_init(&echo, GPIOB, 10);  	// ICAP init as PB10 as input caputre
 	ICAP_counter_us(&echo, 10);   	// ICAP counter step time as 10us
	ICAP_setup(&echo, 3, RISE);   	// TIM2_CH3 as IC3 , rising edge detect
	ICAP_setup(&echo, 4, FALL);     // TIM2_CH3 as IC4 , falling edge detect

// Enable TIMx interrupt			-----------------------------------------------------------------------	
  TIM_INT_enable(TIM2);           // TIM2 Interrupt Enable 
}
