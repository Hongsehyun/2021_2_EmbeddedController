#include "stm32f4xx.h"
#include "stm32f411xe.h"

#include "ecGPIO.h"
#include "ecRCC.h"
#include "ecSysTick.h"
#include "ecEXTI.h"


// LAB4
void SysTick_init(uint32_t msec) {
    //  SysTick Initialiization ------------------------------------------------------            
    //  SysTick Control and Status Register
    SysTick->CTRL = 0;             // Disable SysTick IRQ and SysTick Counter

    // Select processor clock
  	// 1 = processor clock;  0 = external clock   : we use processor CLK
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;
    
    // uint32_t MCU_CLK=EC_SYSTEM_CLK
    // SysTick Reload Value Register
    SysTick->LOAD = ((MCU_CLK_PLL/1000)*msec)-1;             // 1ms 
    // RELOAD = ( Clock Freq[Hz] * Interval[s] ) - 1
    // PLL = 84MHz = 8400,0000Hz
    // We have to choose What we gonna set the Interval.
    // Here, I chose 1ms for interval. ( 1ms = 1/1000sec )
    // If we want use 1sec for interval, then just put 1sec in Upper Formula.
	
    // Clear SysTick Current Value 
    SysTick->VAL = 0;

    // Enables SysTick exception request
    // 1 = counting down to zero asserts the SysTick exception request
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
  
    // Enable SysTick IRQ and SysTick Timer
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
			
    NVIC_SetPriority(SysTick_IRQn, 16);      // Set Priority to 1
    SysTick_enable();                        // Enable interrupt in NVIC
}

void SysTick_us_init(uint32_t usec) {
    SysTick->CTRL = 0;             // Disable SysTick IRQ and SysTick Counter

    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;
    
    SysTick->LOAD = ((MCU_CLK_PLL/1000000)*usec)-1;             // 1us 

    SysTick->VAL = 0;

    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
      
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
			
    NVIC_SetPriority(SysTick_IRQn, 16);       // Set Priority to 1
    SysTick_enable();                        // Enable interrupt in NVIC
}

uint32_t SysTick_val(void){
	return SysTick->VAL;
}

void SysTick_reset(void) {
	SysTick->VAL = 0;
}
    //  SysTick Initialiization ------------------------------------------------------   
void SysTick_enable(void) {
	NVIC_EnableIRQ(SysTick_IRQn);        // Enable interrupt in NVIC
}
 
void SysTick_disable(void) {
  NVIC_DisableIRQ(SysTick_IRQn);       // Disable interrupt in NVIC
}

void delay_ms(uint32_t msec) {
	TimeDelay = msec;
	while(TimeDelay != 0);
}

void delay_us(uint32_t usec) {
	TimeDelay = usec;
	while(TimeDelay != 0);
}

void SysTick_Handler(void) {	
		if(TimeDelay>0)   TimeDelay--;
}
