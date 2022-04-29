/**
******************************************************************************
* @author  SSSLAB
* @Mod		 2021-11-04 by Hong Se Hyun  	
* @brief   Embedded Controller:  Tutorial
* 
******************************************************************************
*/

#include "stm32f411xe.h"
#include "math.h"

#include "ecEXTI.h"
#include "ecGPIO.h"
#include "ecPWM.h"
#include "ecRCC.h"
#include "ecSysTick.h"
#include "ecTIM.h"
#include "ecUART.h"

void setup(void);
void TIM2_IRQHandler(void);

static volatile double ovf_cnt = 0;

static volatile double Step   = 0; 
static volatile double Period = 0; 

static volatile uint8_t print = 0;

static volatile uint8_t capture_start = 0;

static volatile double Start_ARR_val  = 0;
static volatile double End_ARR_val    = 0;
	 

int main(void){
	
	setup();
	
	// TIM configuration -----------------------------------------------------------------------
	GPIOA->AFR[0] |= 1UL<<(4*0);                  // AF1(TIM2)
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;					  // TIM2 Clock enabled

	TIM2->CR1 &= ~TIM_CR1_DIR; 			              // TIM2 counting direction: Up 
	TIM2->PSC = 83;					          				  	// Timer counter clock: 1M`Hz(1us)
	TIM2->ARR = 0XFFFF;    								 	      // Set auto reload register to maximum (count up to 65535)
	TIM2->CR1 |= TIM_CR1_CEN;                     // CR1 DIR enabled

	TIM2->CCMR1 &= ~(3UL<<0); 		                // Capture/Compare Selection: CC1 is mapped on TI1 
	TIM2->CCMR1 |= (1UL<<0); 					            // Capture/Compare Selection: CC1 is mapped on TI1 
	
	TIM2->CCMR1 &= ~(15UL<<4);                    // Clear IC1F
  TIM2->CCMR1 |= 2UL<<4;                        // Set filter N=4

	TIM2->CCER  &= ~(15UL<<0);                    // Clear CCER
  TIM2->CCER  &= ~(5UL<<1);                     // Capture Operation : non-Inverted and rising edge
  
	// Enable Code
	TIM2->CCER |= 1UL<<0;                         // Capture Enable
	TIM2->DIER |= 1UL<<0;                         // Capture/Compare Interrupt Enable
	TIM2->DIER |= 1UL<<1;                         // Update Interrupt Enable

  // Set Interrupt
	NVIC_SetPriority(TIM2_IRQn, 2);						    // Set the priority of TIM2 interrupt request
	NVIC_EnableIRQ(TIM2_IRQn);									  // TIM2 interrupt request enable


  while(1){
	//	if(print==1) {
	//			ovf_cnt   = 0;
	//		  print     = 0;
	      printf("%f[sec]\r\n", (double)Period);
				delay_ms(1000);
	}
}


void TIM2_IRQHandler(void){
	
	if(TIM2->SR & TIM_SR_UIF){                 // Update Interrupt :: Counting the OverFlow
    
		  if(capture_start == 1) ovf_cnt++;
      
		  TIM2->SR &= ~(1UL<<0);                 // clear update interrupt flag
      // clear_UIF(TIM2);                     // clear update interrupt flag
	}


	if((TIM2->SR & TIM_SR_CC1IF) != 0) { 
		// User code to calculate the period of 1Hz pulse
		// ...
		// printf("%f[sec]\r\n", period);         // print out the period on TeraTerm
		
		if(capture_start==1) {
			
			capture_start=0;
			
			End_ARR_val = TIM2->CCR1;
			TIM2->CCR1 = 0;
			
			  if(Start_ARR_val<=End_ARR_val)
				{ 
					Step = ( (End_ARR_val - Start_ARR_val) + ( (TIM2->ARR+1) * ovf_cnt ) );
					Period = Step / 1000000;
				} 
			
			  if(Start_ARR_val>End_ARR_val)
				{
					Step = ( (0xFFFF - Start_ARR_val + End_ARR_val) + ( (TIM2->ARR + 1) * (ovf_cnt-1) ) );
					Period = Step / 1000000;
				}
				
				// print = 1;
				ovf_cnt = 0;
  		}
    
		
		if(capture_start==0) {
			capture_start = 1;
			
			Start_ARR_val = TIM2->CCR1;
			TIM2->CCR1 = 0;
		}	
  
			TIM2->SR &= ~(1UL<<1);
			// clear_CC1IF(TIM2);     // clear capture/compare interrupt flag ( it is also cleared by reading TIM2_CCR1)
	}
}


// Initialiization 
void setup(void)
{	
	RCC_PLL_init();                                   // System Clock = 84MHz
	SysTick_init(1);                                  // Systick init
	UART2_init();                                     // TeraTerm Init
	
  // GPIO configuration ---------------------------------------------------------------------
	GPIO_init(GPIOA, 0, AF);	// PB10: Alternate Function Mode
}
