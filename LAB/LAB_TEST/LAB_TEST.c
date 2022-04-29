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

#include "ecRCC.c"
#include "ecGPIO.c"
#include "ecEXTI.c"
#include "ecSysTick.c"

#define MCU_CLK_PLL 84000000
#define MCU_CLK_HSI 16000000

//PA5, PA6, PA7, PB6, PC7, PA9, PA8, PB10  for DOUT
#define LED_PIN1 	 5
#define LED_PIN2 	 6
#define LED_PIN3 	 7
#define LED_PIN4 	 6
#define LED_PIN5 	 7
#define LED_PIN6 	 9
#define LED_PIN7 	 8
#define LED_PIN8 	 10
#define BUTTON_PIN  13

//GPIO Push-Pull    
#define N_PUPD      0x00
#define PU          0x01
#define PD          0x02
#define Reserved    0x03

//GPIO Speed
#define Low         0x00
#define Medium      0x01
#define Fast        0x02
#define High        0x03

//GPIO Output Type
#define PushPull    0x00
#define OpenDrain   0x01

#define RISING	0
#define FALLING	1

#define PA_x	  0
#define PB_x	  1
#define PC_x	  2 
#define PD_x	  3
#define PE_x  	4
#define PH_x    7


void setup(void);

void EXTI15_10_IRQHandler(void);
void SysTick_Handler(void);


//static volatile uint32_t msTick = 0;
//static volatile uint32_t curTicks;
static volatile uint32_t sysTick_Interval = 1000;
static volatile uint32_t delayValue       = 1000;

//static volatile uint32_t TimeDelay;
static volatile uint32_t currentTime_1;      // Time When Start
static volatile uint32_t currentTime_2;      // Time When Button_Pressed
static volatile uint32_t ExtraDelay;      // Time When Button_Pressed

static volatile uint32_t Flag_ExtraDelay;


int main(void) {
	
  // Initialiization ----------------------------------------
	setup();
	unsigned int cnt = 1;

	
	// Infinite Loop ------------------------------------------
	while(1) {
	
	if(GPIO_read(GPIOC, BUTTON_PIN) == 0) {
		
			 SysTick_reset();
			 currentTime_1 = SysTick_val();
		
		   sevensegment_decode(cnt % 10);
		   delay_ms(delayValue);           // Delay 1000ms = 1sec
			 
			 if(Flag_ExtraDelay==1) {
			 ExtraDelay = sysTick_Interval - (currentTime_2 - currentTime_1);				 
			 
			 }
		   
			 cnt++;
			 if(cnt > 9)   cnt = 0;
	     
		}
	}
}





void EXTI15_10_IRQHandler(void) {
	 currentTime_2 = SysTick_val();
	 if (is_pending_EXTI(BUTTON_PIN)) {
       Flag_ExtraDelay = 1;
       clear_pending_EXTI(BUTTON_PIN);
	}  
}


// SysTick Interrupt Handler - Located in NVIC
// Just Use 'void SysTick_Handler(void)'  Defined, and Do NOT Modify.
// This Function Operates Automatically. Do not need to call in Main.
void SysTick_Handler(void) {	
		if(TimeDelay>0)   TimeDelay--;
}


void setup(void)
{
	//In Simul , CLK Disable must
	RCC_PLL_init();                        // CLK Freq. 84Mhz
	
	//Interrupt Setting  :: Button Interrupt
	EXTI_init(GPIOC, BUTTON_PIN, FALLING, 0);
	//GPIO_init(GPIOC, BUTTON_PIN, INPUT);   // calls RCC_GPIOC_enable()
	//GPIO_pudr(GPIOC, BUTTON_PIN, PU);
	
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
	
	//Initial State == 0, When Reset
	sevensegment_decode(0);
	
	//Interrupt Setting  :: Systick Interrupt
	SysTick_init(sysTick_Interval);	                   // Systick Interrupt
}
