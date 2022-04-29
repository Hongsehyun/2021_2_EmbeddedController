#include "stm32f4xx.h"
#include "stm32f411xe.h"
#include "EC_HAL.h"

void TIM1_IRQHanlder(void);
void SYSTICK_Hanlder(void);
void ADC_IRQHandler(void);
void USART1_IRQHandler(void);
void EXTI0_IRQHandler(void);


void main(void)
{
	setup();
	
	while(1)
	{
	// polling logic goes here
	};
}


void setup()
{
	// Option1: Configure initialization of MCU GPIO
	GPIO_init()
	TIMERx_init()
	PWM_init()
	INT_init()
	ADC_init()
	USART_init()
	//Option 2: you can use the default setup by calling void MCU_init( ) defined in “EC_stm32f411.h?
  // MCU_init( );
}


// For Interrupts,
// 1. Set the priority of each type of interrupt handler
// 2. Make sure interrupts is as short as possible. 
// 3. For a periodic interrupt, check the calculation time within the interrupt handler. It should not go beyond the interrupt period
void TIM1_IRQHanlder(void)   // TIM1~TIM8
{
	// Periodic tasks. Such as
	// Sensor read
  // motor controller out, etc 
	// make it as short as possible
}

void SYSTICK_Hanlder(void)
{
	// Periodic tasks. Such as time measurement etc. 
	// make it as short as possible
}

void ADC_IRQHandler(void)
{
	// Periodic ADC acquisition. 
	// Configure sampling rate with the period of
	// triggering Timer
}

void USART1_IRQHandler(void)   // USART1~USART2
{
	// USART interrupt call whenever data is received
}

void EXTI0_IRQHandler(void)   // EXTI0~15
{
   /* USER CODE BEGIN EXTI0_IRQn 0 */
   /* USER CODE END EXTI0_IRQn 0 */
   /* USER CODE BEGIN EXTI0_IRQn 1 */
   /* USER CODE END EXTI0_IRQn 1 */
}
