/**
******************************************************************************
* @author  SSSLAB
* @Mod       2021-11-17 by SH Hong
* @brief   Embedded Controller: LAB_IR Sensor
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
#include "ecADC.h"


//IR parameter//
static volatile uint16_t flag   =0;

static volatile int IR1 =0;
static volatile int IR2 =0;

static volatile int cmp_val   = 0;
static volatile int threshold = 1000;

void setup(void);
void ADC_IRQHandler(void);

static int seqCHn[2] = {8, 9};


int main(void) { 
   // Initialiization --------------------------------------------------------
   setup();
   
   // Inifinite Loop ----------------------------------------------------------
   while(1) {
		 if(IR1!=0 && IR2!=0){
		   cmp_val = fabs(IR1-IR2);
       printf("IR1 :: %d  \r\n", IR1);
       printf("IR2 :: %d  \r\n", IR2);
     }
		 if( cmp_val > (threshold*2) ){
			 if(IR1>threshold)          printf("GO LEFT \r\n");
       if(IR2>threshold)          printf("GO RIGHT \r\n");
		 }     
		 printf("\n");
     delay_ms(1000);
   }
}


void ADC_IRQHandler(void){
   if( is_ADC_OVR() ) {       //after finishing sequence
      clear_ADC_OVR();
   }
   if( is_ADC_EOC() ) {   
      if(flag==0) {
         IR1 = ADC1->DR;
      }
      else if(flag==1) {
         IR2 = ADC1->DR;
      }
      flag =! flag;
   }
}


// Initialiization 
void setup(void)
{   
   RCC_PLL_init();                         // System Clock = 84MHz
   SysTick_init(1);   
   UART2_init();
   
   ADC_init(GPIOB,0,TRGO, 8);   // ADC_IN8
   ADC_init(GPIOB,1,TRGO, 8);   // ADC_IN9
   
   ADC_TRGO(TIM3, 1, RISE);
   ADC_sequence(2,seqCHn);
   ADC_continue(SINGLE);
   // Enable SW trigger 
   ADC_start(ADC1);
}
