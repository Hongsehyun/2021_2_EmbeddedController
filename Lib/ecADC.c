#include "stm32f411xe.h"
#include "ecSysTick.h"

#include "ecADC.h"
#include "ecGPIO.h"
#include "ecTIM.h"
#include <stdint.h>

uint32_t result;


void ADC_init(GPIO_TypeDef *port, int pin, int trigmode, int prescaler){  //mode 0 : SW, 1 : TRGO
 
	// 0. Match Port and Pin for ADC channel	
	int CHn = ADC_pinmap(port, pin);			// ADC Channel <->Port/Pin mapping

	
  /************************* GPIO configuration *************************/
	/************************* GPIO configuration *************************/
  // 1. Initialize GPIO port and pin as ANALOG, no pull up / pull down
  GPIO_init(port, pin, ANALOG);  				// ANALOG = 3
	GPIO_pudr(port, pin, N_PUPD);  		  	// EC_NONE = 0

	/************************* ADC configuration *************************/
	/************************* ADC configuration *************************/
  // 1. Total time of conversion setting
  // Enable ADC pheripheral clock
	RCC->APB2ENR  |= RCC_APB2ENR_ADC1EN; 		// Enable the clock of RCC_APB2ENR_ADC1EN
	
	// Configure ADC clock pre-scaler
	ADC->CCR &= ~(ADC_CCR_ADCPRE);					// clear prescaler = 0000: PCLK2 divided by 2	(42MHz)
	ADC->CCR |= (uint16_t) (((prescaler/2)-1)<<16);					
	
	// Configure ADC resolution 
	ADC1->CR1 &= ~ADC_CR1_RES;          		// 00: 12-bit resolution (15cycle+)
	
	// Configure channel sampling time of conversion.	
	// Software is allowed to write these bits only when ADSTART=0 and JADSTART=0	!!
	// ADC clock cycles @42MHz = 2us
	if(CHn < 10) {
		ADC1->SMPR2 &= ~(7U << (3*CHn));
		ADC1->SMPR2 |= (4U << (3*CHn));					// sampling time conversion : 84  			
	}
	
	else {
  	ADC1->SMPR2 &= ~(7U << (3*(CHn%10)));
		ADC1->SMPR1 |= (4U << (3*(CHn%10)));
	}	
	
  // 2. Regular / Injection Group 
	//Regular: SQRx, Injection: JSQx

  // 3. Repetition: Single or Continuous conversion
	ADC1->CR2 &= ~(ADC_CR2_CONT);      	  	// Enable Single conversion mode	
	//ADC1->CR2 |= ADC_CR2_CONT;      		   	// Enable Continuous conversion mode	
	
  // 4. Single Channel or Scan mode
	//  - Single Channel: scan mode, right alignment
	ADC1->CR1 |= ADC_CR1_SCAN;					  	// 1: Scan mode enable 
	ADC1->CR2 &= ~(ADC_CR2_ALIGN);   				// 0: Right alignment	
	
	// Configure the sequence length
	ADC1->SQR1 &= ~(ADC_SQR1_L); 				  	// 0000: 1 conversion in the regular channel conversion sequence
	
	// Configure the channel sequence 
	ADC1->SQR3 &= ~(ADC_SQR3_SQ1);				 	// SQ1 clear bits
	ADC1->SQR3 |= (CHn & ADC_SQR3_SQ1);     // Choose the channel to convert firstly

  // 5. Interrupt Enable
	// Enable EOC(conversion) interrupt. 
	ADC1->CR1 &= ~(ADC_CR1_EOCIE);          // Interrupt reset
	ADC1->CR1 |= ADC_CR1_EOCIE;             // Interrupt enable
	
	// Enable ADC_IRQn 
	NVIC_SetPriority(ADC_IRQn,2); 			// Set Priority to 2
	NVIC_EnableIRQ(ADC_IRQn);      	// Enable interrupt form ACD1 peripheral	


	/************************* HW TRIGGER MODE *************************/
	/************************* HW TRIGGER MODE *************************/
	// TRGO Initialize : TIM3, 1msec, RISE edge
	if(trigmode==TRGO) ADC_TRGO(TIM3, 1, RISE);				
}


void ADC_TRGO(TIM_TypeDef* TIMx, int msec, int edge){
	// set timer
	int timer = 0;
	if(TIMx==TIM2)      timer=2;
	else if(TIMx==TIM3) timer=3;	
	
	// Single conversion mode (disable continuous conversion)
	// ADC1->CR2 &= ~ADC_CR2_SWSTART;     			// Discontinuous conversion mode
	ADC1->CR2 &= ~(1<<1);
	ADC1->CR2 |= ADC_CR2_EOCS;  				       	// Enable EOCS

  // HW Trigger configuration -------------------------------------------------------------
  	
  // 1. TIMx Trigger Output Config
	// Enable TIMx Clock
	TIM_init(TIMx, msec);
	TIMx->CR1 &= ~TIM_CR1_CEN; 			  		//counter disable
	
	// Set PSC, ARR
  TIM_period_ms(TIMx, msec);
	
  // Master Mode Selection MMS[2:0]: Trigger output (TRGO)
  TIMx->CR2 &= ~TIM_CR2_MMS; 					  // reset MMS
  TIMx->CR2 |= TIM_CR2_MMS_2;   				// 100: Compare - OC1REF signal is used as trigger output (TRGO)
   
	// Output Compare Mode
  TIMx->CCMR1 &= ~(7<<4);       	  		// OC1M : output compare 1 Mode 
  TIMx->CCMR1 |= 6<<4;            			// OC1M = 110 for compare 1 Mode ch1 
	
  // OC1 signal 
  TIMx->CCER |= TIM_CCER_CC1E;          // CC1E Capture enabled
	TIMx->CCR1  = (TIMx->ARR)/2; 		  		// duty ratio 50%
   
  // Enable TIMx 
  TIMx->CR1 |= TIM_CR1_CEN; 				  	// counter enable

  // 2. ADC HW Trigger Config.
	// Select Trigger Source  			
	ADC1->CR2 &= ~ADC_CR2_EXTSEL; 		  	// reset EXTSEL
	ADC1->CR2 |= (timer*2+2)<<24; 		  	// TIMx TRGO event (ADC : TIM2, TIM3 TRGO)
	
	//Select Trigger Polarity
	ADC1->CR2 &= ~ADC_CR2_EXTEN;			  	              // reset EXTEN, default
	if(edge==RISE) ADC1->CR2 |= ADC_CR2_EXTEN_0;				// trigger detection rising edge
	else if(edge==FALL) ADC1->CR2 |= ADC_CR2_EXTEN_1;		// trigger detection falling edge
	else if(edge==BOTH) ADC1->CR2 |= ADC_CR2_EXTEN_Msk;	// trigger detection both edge
}


void ADC_continue(int contmode){
	if(contmode==CONT){
		// Repetition: Continuous conversion
		ADC1->CR2 |= ADC_CR2_CONT;      	// Enable Continuous conversion mode	
		ADC1->CR1 &= ~ADC_CR1_SCAN;				// 0: Scan mode disable 
		// Summary :: Single Channel, Continuous Conversion mode
		// Summary :: Single Channel, Continuous Conversion mode
	}
	else 																//if(contmode==SINGLE)
		{
		// Repetition: Single conversion
		// For HW Trigger, we must DISABLE CONTINUOUS CONVERSION and set to SINGLE CONVERSION MODE
		ADC1->CR2 &= ~ADC_CR2_CONT;      	// Disable Continuous conversion mode	
		ADC1->CR1 |= ADC_CR1_SCAN;				// 1: Scan mode enable
		// Summary :: Multiple Channel(Scan mode), Single Conversion mode
		// Summary :: Multiple Channel(Scan mode), Single Conversion mode
	}
} 


void ADC_sequence(int length, int *seq){
	// Regular channel Sequence Length
	ADC1->SQR1 &= ~(0xF<<20); 						// reset length of conversions in the regular channel 	
	ADC1->SQR1 |= (length-1)<<20; 				// conversions in the regular channel conversion sequence
	
	//if (length>1) ADC1->CR1 |= ADC_CR1_SCAN;
	for(int i = 0; i<length; i++){
		if (i<6){
			ADC1->SQR3 &= ~(0x1F<<i*5);				// SQn clear bits
			ADC1->SQR3 |= seq[i]<<i*5;				// Choose the channel to convert sequence
		}
		else if (i <12){
			ADC1->SQR2 &= ~(0x1F<<((i-6)*5));      // SQn clear bits
			ADC1->SQR2 |= seq[i]<<((i-6)*5);		  // Choose the channel to convert sequence
		}
		else{
			ADC1->SQR1 &= ~(0x1F<<(i-12)*5);	// SQn clear bits
			ADC1->SQR1 |= seq[i]<<(i-12)*5;		// Choose the channel to convert sequence
		}
	}
}


void ADC_start(ADC_TypeDef *ADCx){
	// Enable ADON, SW Trigger-------------------------------------------------------------------------------
	ADCx->CR2 |= ADC_CR2_ADON;
	ADCx->CR2 |= ADC_CR2_SWSTART;
}


// Check :: Conversion complete (EOCS=0), or sequence of conversions complete (EOCS=1)
uint32_t is_ADC_EOC(void){
	//return ((ADC1->SR & ADC_SR_EOC) == ADC_SR_EOC);
	return ADC1->SR & ADC_SR_EOC;
}


// Check :: Overrun has occurred
// When Over-run Detect, Program gonna be stop.
// Therefore, We should clear the Over-run bit when Over-run Detected.
uint32_t is_ADC_OVR(void){
	//return ((ADC1->SR & ADC_SR_OVR) == ADC_SR_OVR);
  return ADC1->SR & ADC_SR_OVR; 
}


void clear_ADC_OVR(void){
	ADC1->SR &= ~ADC_SR_OVR;
}


uint32_t ADC_read(void){
	return ADC1->DR & 0xFFFF;
}


uint32_t ADC_pinmap(GPIO_TypeDef *Port, int Pin){
	if(Port == GPIOA){
		if(Pin == 0) 			return 0;
		else if(Pin == 1) return 1;
		else if(Pin == 4) return 4;
		else if(Pin == 5) return 5;
		else if(Pin == 6) return 6;
		else if(Pin == 7) return 7;
		else 							while(1);
	}
	else if(Port == GPIOB){
		if(Pin == 0) 			return 8;
		else if(Pin == 1)	return 9;
		else 							while(1);
	}
	else if(Port == GPIOC){
		if(Pin == 0)			return 10;
		else if(Pin == 1)	return 11;
		else if(Pin == 2)	return 12;
		else if(Pin == 3)	return 13;
		else if(Pin == 4)	return 14;
		else if(Pin == 5)	return 15;
		else							while(1);
	}
}
