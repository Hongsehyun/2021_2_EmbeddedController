/**
******************************************************************************
* @author  SSSLAB
* @Mod     2021-10-15 by SHHnog
* @brief   Embedded Controller:  Tutorial ___SysTick and EXTI
*
******************************************************************************
*/

#include "stm32f4xx.h"
#include "ecRCC.h"
#include "ecGPIO.h"

#define MCU_CLK_PLL 84000000
#define MCU_CLK_HSI 16000000

#define LED_PIN     5
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

#define SYSCFG_EXTICR4_EXTI13_PA             0x0000U                           /*!<PA[13] pin */
#define SYSCFG_EXTICR4_EXTI13_PB             0x0010U                           /*!<PB[13] pin */
#define SYSCFG_EXTICR4_EXTI13_PC             0x0020U                           /*!<PC[13] pin */
#define SYSCFG_EXTICR4_EXTI13_PD             0x0030U                           /*!<PD[13] pin */
#define SYSCFG_EXTICR4_EXTI13_PE             0x0040U                           /*!<PE[13] pin */


void setup(void);

void EXTI15_10_IRQHandler(void);
void SysTick_Handler(void);

volatile uint32_t msTick = 0;
volatile uint32_t curTicks;
volatile uint32_t msDelay=10;


int main(void) {

/*
   // Tutorial 1. EXTI Register Initiation
   // System CLOCK, GPIO Initialiization ----------------------------------------
   setup();

   // EXTI Initialiization ------------------------------------------------------   
   // SYSCFG peripheral clock enable
   RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

   // Connect External Line to the GPIO
   // Button: PC_13 -> EXTICR3(EXTI13)
   SYSCFG->EXTICR[BUTTON_PIN/4] &= ~SYSCFG_EXTICR4_EXTI13;
   SYSCFG->EXTICR[BUTTON_PIN/4] |= SYSCFG_EXTICR4_EXTI13_PC;

   // Falling trigger enable (Button: pull-up)
   EXTI->FTSR |= 1UL << BUTTON_PIN;

   // Unmask (Enable) EXT interrupt
   EXTI->IMR |= 1UL << BUTTON_PIN;

   // Interrupt IRQn, Priority
   NVIC_SetPriority(EXTI15_10_IRQn, 0);        // Set EXTI priority as 0   
   NVIC_EnableIRQ(EXTI15_10_IRQn);          // Enable EXTI 

   while (1);
*/



    // Tutorial 2. SYSTICK
    // System CLOCK, GPIO Initialiization ----------------------------------------
    setup();
      
    // SysTick Initialiization ------------------------------------------------------            
    // SysTick Control and Status Register
    SysTick->CTRL = 0;             // Disable SysTick IRQ and SysTick Counter

    // Select processor clock
    // 1 = processor clock;  0 = external clock
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;
    
    // uint32_t MCU_CLK=EC_SYSTEM_CLK
    // SysTick Reload Value Register
    SysTick->LOAD = (MCU_CLK_PLL/1000)-1;             // 1ms
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
    NVIC_EnableIRQ(SysTick_IRQn);               // Enable interrupt in NVIC
      
         
   // While loop ------------------------------------------------------            
    msTick = 0;

    while(1){
      
    // Method 1      
       curTicks = msTick;
       while ((msTick - curTicks) < 100);
       LED_toggle(GPIOA, LED_PIN);
       msTick = 0;
    /*
    // Method 2      
       if ((msTick) > 1000){
          LED_toggle(GPIOA, LED_PIN);         
          msTick = 0;
       }
         
    // Method 3            
       msDelay=1000;
       while(msDelay !=0);
       LED_toggle(GPIOA, LED_PIN);
		 }
		*/
}

}



void EXTI15_10_IRQHandler(void) {
    // same as  ( EXTI->PR & 1 ) == 1  
   if ((EXTI->PR & EXTI_PR_PR13) == EXTI_PR_PR13) {
       LED_toggle(GPIOA, LED_PIN);
     EXTI->PR |= EXTI_PR_PR13; // cleared by writing '1'
   }
}



// SysTick Interrupt Handler - Located in NVIC
// Just Use 'void SysTick_Handler(void)'  Defined, and Do NOT Modify.
// This Function Operates Automatically. Do not need to call in Main.
void SysTick_Handler(void) {
      msTick++;
      msDelay--;
}



// Initialiization 
void setup(void)
{
   RCC_PLL_init();                       // System Clock = 84MHz
   // RCC_HSI_init();
   // Initialize GPIOA_5 for Output
   
   GPIO_init(GPIOA, LED_PIN, OUTPUT);    // calls RCC_GPIOA_enable()   
   // Initialize GPIOC_13 for Input Button
   
   //GPIO_init(GPIOC, BUTTON_PIN, INPUT);  // calls RCC_GPIOC_enable()
}
