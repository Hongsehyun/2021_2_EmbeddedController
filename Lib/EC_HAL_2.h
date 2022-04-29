// JW
/*
#ifndef __MY_ADC_H
#define __MY_ADC_H
#include "stm32f411xe.h"

// ADC trigmode
#define SW 0
#define TRGO 1

// ADC contmode
#define CONT 0
#define SINGLE 1

// Edge Type
#define RISE_ADC 1
#define FALL_ADC 2
#define BOTH_ADC 3

#define _DEFAULT 0

// ADC setting
void ADC_init(GPIO_TypeDef *port, int pin, int trigmode);         // trigmode : SW , TRGO
void ADC_continue(int contmode);                                        // contmode : CONT, SINGLE / Operate both ADC,JADC
void ADC_TRGO(TIM_TypeDef* TIMx, uint32_t msec, int edge);
void ADC_sequence(int length, int *seq); 
void TIM_TRGO_init(TIM_TypeDef* timx, uint32_t msec);

void ADC_inject_init(GPIO_TypeDef *port, int pin, int trigmode);
void ADC_inject_TRGO(TIM_TypeDef* TIMx, uint32_t msec, int edge);
void ADC_inject_sequence(uint16_t length, uint16_t *seq);
void ADC_inject_start(void);
void ADC_start(void);
void clear_ADC_JEOC(void);
uint32_t is_ADC_JEOC(void);


uint32_t is_ADC_EOC(void);
uint32_t is_ADC_OVR(void);
void clear_ADC_OVR(void);

uint32_t ADC_read(void);
uint32_t ADC_read_JDR(uint32_t num);
uint32_t ADC_pinmap(GPIO_TypeDef *port, int pin);
#endif
*/






/*
#include "stm32f411xe.h"
#include "ecSystick.h"
#include "ecADC.h"
#include "ecGPIO.h"
#include "ecTIM.h"

uint32_t result;


void ADC_init(GPIO_TypeDef *port, int pin, int trigmode){  //mode 0 : SW, 1 : TRGO
// 0. Match Port and Pin for ADC channel   
   uint32_t CHn = ADC_pinmap(port, pin);         // ADC Channel <->Port/Pin mapping

// GPIO configuration ---------------------------------------------------------------------   
// 1. Initialize GPIO port and pin as ANALOG, no pull up / pull down
   GPIO_init(port, pin, ANALOG);              // ANALOG = 3
   GPIO_pudr(port, pin, NO_PUPD);           // EC_NONE = 0

// ADC configuration   ---------------------------------------------------------------------         
// 1. Total time of conversion setting
   // Enable ADC pheripheral clock
   RCC->APB2ENR  |= RCC_APB2ENR_ADC1EN;       // Enable the clock of RCC_APB2ENR_ADC1EN
   
   // Configure ADC clock pre-scaler
   ADC->CCR &= ~(3 << 16);               
   ADC->CCR |= (3 << 16);         // 0000: PCLK2 divided by 8   (42MHz)
   
   // Configure ADC resolution 
   ADC1->CR1 &=  ~ (3 << 24);           // 00: 12-bit resolution (15cycle+)
   
   // Configure channel sampling time of conversion.   
   // Software is allowed to write these bits only when ADSTART=0 and JADSTART=0   !!
   // ADC clock cycles @42MHz = 2us
   
   if(CHn < 10) 
   {
      ADC1->SMPR2  &= ~(7U << CHn*3);
      ADC1->SMPR2  |= 4U << CHn*3;               // sampling time conversion : 84           
   
   }   
      else             
   {
    ADC1->SMPR1  &= ~(7U << (CHn%10)*3);      
      ADC1->SMPR1  |= 4U << (CHn%10)*3;
   }
// 2. Regular / Injection Group 
   //Regular: SQRx, Injection: JSQx

// 3. Repetition: Single or Continuous conversion
   ADC1->CR2 |= ADC_CR2_CONT; //must be change single               // Enable Continuous conversion mode   
   
// 4. Single Channel or Scan mode
   //  - Single Channel: scan mode, right alignment
   // use default
   
   ADC1->CR1 |= 1 << 8 ;                  // 1: Scan mode enable 
   //ADC1->CR2 &= ___________;               // 0: Right alignment   
   
   // Configure the sequence length
   ADC1->SQR1 &=  ~(0xF<<20);               // 0000: 1 conversion in the regular channel conversion sequence
   
   // Configure the channel sequence 
   ADC1->SQR3 &= ~ (0x1F << CHn*5);                // SQ1 clear bits
   ADC1->SQR3 |= (CHn & 0x1F);    // Choose the channel to convert firstly
      
   
// 5. Interrupt Enable
   // Enable EOC(conversion) interrupt. 
   ADC1->CR1 &=  ~(1<<5);             // Interrupt reset
   ADC1->CR1 |=  (1<<5);          // Interrupt enable
   
   // Enable ADC_IRQn 
   NVIC_SetPriority(ADC_IRQn,1);          // Set Priority to 2
   NVIC_EnableIRQ(ADC_IRQn);         // Enable interrupt form ACD1 peripheral   

   // TRGO Initialize : TIM3, 1msec, RISE edge
   if(trigmode==TRGO) ADC_TRGO(TIM3, 100, RISE_ADC);            
   
}
void ADC_inject_init(GPIO_TypeDef *port, int pin, int trigmode){  //mode 0 : SW, 1 : TRGO
// 0. Match Port and Pin for ADC channel   
   uint32_t CHn = ADC_pinmap(port, pin);         // ADC Channel <->Port/Pin mapping

// GPIO configuration ---------------------------------------------------------------------   
// 1. Initialize GPIO port and pin as ANALOG, no pull up / pull down
   GPIO_init(port, pin, ANALOG);              // ANALOG = 3
   GPIO_pudr(port, pin, NO_PUPD);           // EC_NONE = 0

// ADC configuration   ---------------------------------------------------------------------         
// 1. Total time of conversion setting
   // Enable ADC pheripheral clock
   RCC->APB2ENR  |= RCC_APB2ENR_ADC1EN;       // Enable the clock of RCC_APB2ENR_ADC1EN
   
   // Configure ADC clock pre-scaler
   ADC->CCR &= ~(3 << 16);               
   ADC->CCR |= (3 << 16);         // 0000: PCLK2 divided by 8   (42MHz)
   
   // Configure ADC resolution 
   ADC1->CR1 &=  ~ (3 << 24);           // 00: 12-bit resolution (15cycle+)
   
   // Configure channel sampling time of conversion.   
   // Software is allowed to write these bits only when ADSTART=0 and JADSTART=0   !!
   // ADC clock cycles @42MHz = 2us
   
   if(CHn < 10) 
   {
      ADC1->SMPR2  &= ~(7U << CHn*3);
      ADC1->SMPR2  |= 4U << CHn*3;               // sampling time conversion : 84           
   
   }   
      else             
   {
    ADC1->SMPR1  &= ~(7U << (CHn%10)*3);      
      ADC1->SMPR1  |= 4U << (CHn%10)*3;
   }
// 2. Regular / Injection Group 
   //Regular: SQRx, Injection: JSQx

// 3. Repetition: Single or Continuous conversion
   ADC1->CR2 &= ~ADC_CR2_CONT; //must be change single               // Enable Continuous conversion mode   
   
// 4. Single Channel or Scan mode
//  - Single Channel: scan mode, right alignment
// use default
   
   ADC1->CR1 |= 1 << 8 ;                  // 1: Scan mode enable 
   //ADC1->CR2 &= ___________;      // 0: Right alignment   
      
   
// 5. Interrupt Enable
   // Enable EOC(conversion) interrupt. 
   ADC1->CR1 &=  ~(1<<7);             // Interrupt reset
   ADC1->CR1 |=  (1<<7);          // Interrupt enable
   
   // Enable ADC_IRQn 
   NVIC_SetPriority(ADC_IRQn,1);          // Set Priority to 2
   NVIC_EnableIRQ(ADC_IRQn);         // Enable interrupt form ACD1 peripheral   

   
   // TRGO Initialize : TIM3, 1msec, RISE edge
   if(trigmode==TRGO) ADC_inject_TRGO(TIM4, 100, RISE_ADC);            
   
}

void ADC_TRGO(TIM_TypeDef* TIMx, uint32_t msec, int edge){
   // set timer
   int timer = 0;
   if(TIMx==TIM2) timer=2;
   else if(TIMx==TIM3) timer=3;   
   
   // Single conversion mode (disable continuous conversion)
   ADC1->CR2 &= ~ADC_CR2_ADON;              // Discontinuous conversion mode
   ADC1->CR2 |= ADC_CR2_EOCS;                 // Enable EOCS
   

   // HW Trigger configuration -------------------------------------------------------------
   
// 1. TIMx Trigger Output Config
   // Enable TIMx Clock
   TIM_TRGO_init(TIMx, msec);
   TIMx->CR1 &= ~TIM_CR1_CEN;                //counter disable
   
   // Set PSC, ARR
  TIM_period_ms(TIMx, msec);
   
  // Master Mode Selection MMS[2:0]: Trigger output (TRGO)
  TIMx->CR2 &= ~ TIM_CR2_MMS ;                // reset MMS(master mode selection)
  TIMx->CR2 |= TIM_CR2_MMS_2;               //100: Compare - OC1REF signal is used as trigger output (TRGO)
   
   // Output Compare Mode
  TIMx->CCMR1 &= ~(7<<4);                // OC1M : output compare 1 Mode 
  TIMx->CCMR1 |= 6<<4;                   // OC1M = 110 for compare 1 Mode ch1 
   
  // OC1 signal 
  TIMx->CCER |= TIM_CCER_CC1E;          // CC1E Capture enabled
   TIMx->CCR1  = (TIMx->ARR)/2;             // duty ratio 50%
   
  // Enable TIMx 
  TIMx->CR1 |= TIM_CR1_CEN;                //counter enable

// 2. ADC HW Trigger Config.
   // Select Trigger Source           
   ADC1->CR2 &= ~ADC_CR2_EXTSEL;          // reset EXTSEL
   ADC1->CR2 |= (timer*2+2)<<24;          // TIMx TRGO event (ADC : TIM2, TIM3 TRGO)
   
   //Select Trigger Polarity
   ADC1->CR2 &= ~ADC_CR2_EXTEN;            // reset EXTEN, default
   if(edge==RISE_ADC) ADC1->CR2 |= ADC_CR2_EXTEN_0;            // trigger detection rising edge
   else if(edge==FALL_ADC) ADC1->CR2 |= ADC_CR2_EXTEN_1;      // trigger detection falling edge
   else if(edge==BOTH_ADC) ADC1->CR2 |= ADC_CR2_EXTEN_Msk;   // trigger detection both edge

}


void ADC_inject_TRGO(TIM_TypeDef* TIMx, uint32_t msec, int edge){
   // set timer
   
   
   // Single conversion mode (disable continuous conversion)
   ADC1->CR2 &= ~ADC_CR2_ADON;              // Discontinuous conversion mode
   ADC1->CR2 |= ADC_CR2_EOCS;                 // Enable EOCS
   

   // HW Trigger configuration -------------------------------------------------------------
   
// 1. TIMx Trigger Output Config
   // Enable TIMx Clock
   TIM_TRGO_init(TIMx, msec);
   TIMx->CR1 &= ~TIM_CR1_CEN;                //counter disable
   
   // Set PSC, ARR
  TIM_period_ms(TIMx, msec);
   
  // Master Mode Selection MMS[2:0]: Trigger output (TRGO)
  TIMx->CR2 &= ~ TIM_CR2_MMS ;                // reset MMS(master mode selection)
  TIMx->CR2 |= TIM_CR2_MMS_2;               //100: Compare - OC1REF signal is used as trigger output (TRGO)
   
   // Output Compare Mode
  TIMx->CCMR1 &= ~(7<<4);                // OC1M : output compare 1 Mode 
  TIMx->CCMR1 |=    6<<4;                   // OC1M = 110 for compare 1 Mode ch1 
   
  // OC1 signal 
  TIMx->CCER |= TIM_CCER_CC1E;          // CC1E Capture enabled
   TIMx->CCR1  = (TIMx->ARR)/2;             // duty ratio 50%
   
  // Enable TIMx 
  TIMx->CR1 |= TIM_CR1_CEN;                //counter enable

// 2. ADC HW Trigger Config.
   // Select Trigger Source           
   ADC1->CR2 &= ~ADC_CR2_JEXTSEL;          // reset EXTSEL
   if(TIMx == TIM1) ADC1->CR2 |= 1<<16;
   else if(TIMx == TIM2) ADC1->CR2 |= 3<<16;         
   else if(TIMx == TIM4) ADC1->CR2 |= 9<<16;
   else if(TIMx == TIM5) ADC1->CR2 |= 11<<16;
   
   ADC1->CR2 &= ~ADC_CR2_JEXTEN;            // reset EXTEN, default
   if(edge==RISE_ADC) ADC1->CR2 |= ADC_CR2_JEXTEN_0;            // trigger detection rising edge
   else if(edge==FALL_ADC) ADC1->CR2 |= ADC_CR2_JEXTEN_1;      // trigger detection falling edge
   else if(edge==BOTH_ADC) ADC1->CR2 |= ADC_CR2_JEXTEN_Msk;   
}


void ADC_continue(int contmode){
   if(contmode==CONT){
      // Repetition: Continuous conversion
      ADC1->CR2 |= ADC_CR2_CONT;         // Enable Continuous conversion mode   
      ADC1->CR1 &= ~ADC_CR1_SCAN;            // 0: Scan mode disable 
   }
   else                                                 //if(contmode==SINGLE)
      {
      // Repetition: Single conversion
      ADC1->CR2 &= ~ADC_CR2_CONT;         // Disable Continuous conversion mode   
      ADC1->CR1 |= ADC_CR1_SCAN;            // 1: Scan mode enable
   }
} 

void ADC_sequence(int length, int *seq){
   
   ADC1->SQR1 &= ~(0xF<<20);                   // reset length of conversions in the regular channel    
   ADC1->SQR1 |= (length-1)<<20;             // conversions in the regular channel conversion sequence
   
   for(int i = 0; i<length; i++){
      if (i<6){
         ADC1->SQR3 &= ~(0x1F<<i*5);            // SQn clear bits
         ADC1->SQR3 |= seq[i]<<i*5;            // Choose the channel to convert sequence
      }
      else if (i <12){
         ADC1->SQR2 &= ~(0x1F<<(i-6)*5);            // SQn clear bits
         ADC1->SQR2 |=seq[i]<<(i-6)*5;            // Choose the channel to convert sequence
      }
      else{
         ADC1->SQR1 &= ~(0x1F<<(i-12)*5);   // SQn clear bits
         ADC1->SQR1 |= seq[i]<<(i-12)*5;      // Choose the channel to convert sequence
      }
   }
}

void ADC_inject_sequence(uint16_t length, uint16_t *seq)
{
   ADC1->JSQR &= ~(3<<20);                   // reset length of conversions in the regular channel    
   ADC1->JSQR |= (length-1)<<20;
   
   for(int i =0; i<length ;i++){
      ADC1->JSQR |= (seq[i]<<5*(3-i));
   }
}


void TIM_TRGO_init(TIM_TypeDef* timx, uint32_t msec)
{
      TIM_init(timx, msec);
}


void ADC_start(void){
   // Enable ADON, SW Trigger-------------------------------------------------------------------------------
   ADC1->CR2 |= ADC_CR2_ADON;
   ADC1->CR2 |= ADC_CR2_SWSTART;
}

void ADC_inject_start(void){
   // Enable ADON, SW Trigger-------------------------------------------------------------------------------
   ADC1->CR2 |= ADC_CR2_ADON;
   ADC1->CR2 |= ADC_CR2_JSWSTART;
}


uint32_t is_ADC_EOC(void){
   return (ADC1->SR & 1<<1);
}

uint32_t is_ADC_OVR(void){ //next by EOC
   return ((ADC1->SR & ADC_SR_OVR) == ADC_SR_OVR);
}

void clear_ADC_OVR(void){
   ADC1->SR &= ~ADC_SR_OVR;
}

uint32_t ADC_read(){
   return ADC1->DR;
}

uint32_t is_ADC_JEOC(void){
   return (ADC1->SR & 2<<1);
}

void clear_ADC_JEOC(void){
   ADC1->SR &= ~ADC_SR_JEOC;
}

uint32_t ADC_read_JDR(uint32_t num){
      
   if(num == 1) return ADC1->JDR1; 
   else if (num == 2) return ADC1->JDR2; 
   else if (num == 3) return ADC1->JDR3; 
   else if (num == 4) return ADC1->JDR4; 
 
   return 0;
}

uint32_t ADC_pinmap(GPIO_TypeDef *Port, int Pin){
   if(Port == GPIOA){
      if(Pin == 0)          return 0;
      else if(Pin == 1) return 1;
      else if(Pin == 4) return 4;
      else if(Pin == 5) return 5;
      else if(Pin == 6) return 6;
      else if(Pin == 7) return 7;
      else                      while(1);
   }
   else if(Port == GPIOB){
      if(Pin == 0)          return 8;
      else if(Pin == 1)   return 9;
      else                      while(1);
   }
   else if(Port == GPIOC){
      if(Pin == 0)         return 10;
      else if(Pin == 1)   return 11;
      else if(Pin == 2)   return 12;
      else if(Pin == 3)   return 13;
      else if(Pin == 4)   return 14;
      else if(Pin == 5)   return 15;
      else                     while(1);
   }
}
*/

















// KS
/*
// InJection Groups
void ADC_Jinit(GPIO_TypeDef *port, int pin, int trigmode, int Prescaler){  //trigmode 0 : SW, 1 : TRGO

   int CHn = ADC_pinmap(port, pin);         // ADC Channel <->Port/Pin mapping
   GPIO_init(port, pin, ANALOG); 
   GPIO_pupdr(port, pin, NPUPD);
   
	// 1. Total time of conversion setting
  //   - Enable peripheral clock
  //  - Configure Prescaler
  //   -   Configure Resolution
  //   -   Channel sampling time of conversion 
   
   RCC->APB2ENR  |= RCC_APB2ENR_ADC1EN;       // Enable the clock of RCC_APB2ENR_ADC1EN                   (PM 121p)
   
   if (Prescaler == 2)         ADC->CCR &=~ADC_CCR_ADCPRE;                             // 0000: PCLK2 divided by 2   (42MHz)            (PM 238p)
   else if(Prescaler ==4)      ADC->CCR |=ADC_CCR_ADCPRE_0;                           // 0001: PCLK2 divided by 4   (21MHz)            (PM 238p)
   else if(Prescaler ==6)      ADC->CCR |=ADC_CCR_ADCPRE_1;                           // 0010: PCLK2 divided by 6   (14MHz)             (PM 238p)
   else if(Prescaler ==8)      ADC->CCR |=ADC_CCR_ADCPRE_1;                              // 0011: PCLK2 divided by 8   (10.5MHz)        (PM 238p)
   
   ADC1->CR1 &= ~ADC_CR1_RES;                    // 00: 12-bit resolution (15cycle+) -> sampling 3(minimum) + resolution 12 = 15 (PM 229p)
   
   // Software is allowed to write these bits only when ADSTART=0 and JADSTART=0   !!
   // ADC clock cycles @42MHz = 2us
   ADC1_SMPR(CHn); // tutorial set 84cycles.

// 2. Regular / Injection Group 
   //Regular: SQRx, Injection: JSQx

// 3. Repetition: Single or Continuous conversion
   // Prof intoduce to me that single mode -> defualt
   ADC1->CR2 &= ~ADC_CR2_CONT;               // default : Single conversion mode  (RM 231p)
   
   
// 4. Single Channel or Scan mode
   ADC1->CR1  |= ADC_CR1_SCAN;                  // 1: Scan mode enable                                                                    (RM 229p)
   ADC1->CR2  &= ~ADC_CR2_ALIGN;            // 0: Right alignment                                                                        (RM 231p)
   ADC1->SQR1 &= ~ADC_SQR1_L;                  // 0000: 1 conversion in the regular channel conversion sequence      (RM 235p)

  // Configure the channel sequence  = CHn conversion in regular sequence                                   (RM 236p)
   ADC1->JSQR &= ~ADC_JSQR_JL;                  // SQ1 clear bits
   ADC1->JSQR |= (CHn  & ADC_JSQR_JSQ1);    // Choose the channel to convert firstly, Later we change it
  
   ADC1_JEOC_Enable();                    // EOC Interrupt enable
   
   NVIC_SetPriority(ADC_IRQn,2);            // Set Priority to 2
   NVIC_EnableIRQ(ADC_IRQn);              // Enable interrupt form ACD1 peripheral   


   
   // TRGO Initialize : TIM4, 1msec, RISE edge  why? 
   // TRGO -> trigger out event 
   if(trigmode==TRGO) ADC_JTRGO(TIM4, 1, RISE);            
}



void ADC_JTRGO(TIM_TypeDef* TIMx, uint32_t msec, int edge){ // Rise or Fall    TRGO serve only TIM2,TIM3
   // set timer
   int timer = 0;
   if(TIMx==TIM1) timer=1;
   else if(TIMx==TIM2) timer=2;   
   else if(TIMx==TIM4) timer=4;   
   else if(TIMx==TIM5) timer=5;   
   
   // TRGO -> only Single mode!!
   // Single conversion mode (disable continuous conversion)
   ADC1->CR2 &= ~ADC_CR2_CONT;                                   // Discontinuous conversion mode        (RM 231p)
   ADC1->CR2 |= ADC_CR2_EOCS;                                      // Enable EOCS                                       (RM 231p)
   

// HW Trigger configuration -------------------------------------------------------------
   
// 1. TIMx Trigger Output Config
   // Enable TIMx Clock
   TIM_init(TIMx, msec); //1msec -> 1Khz
   TIMx->CR1 &= ~TIM_CR1_CEN;                                        //counter disable                       (RM 352p)
   
   // Set PSC, ARR
  TIM_period_ms(TIMx, msec);
   
  // Master Mode Selection MMS[2:0]: Trigger output (TRGO)
  TIMx->CR2 &= ~TIM_CR2_MMS;                                        // reset MMS
  TIMx->CR2 |= TIM_CR2_MMS_2;                                    //100: Compare    - OC1REF signal of TIMx is used as trigger output (TRGO)
   
   // Output Compare Mode
  TIMx->CCMR1 &= ~(7<<4);                                        // OC1M : output compare 1 Mode 
  TIMx->CCMR1 |= 6<<4;                                           // OC1M = 110 for compare 1 Mode ch1 
   
  // OC1 signal 
  TIMx->CCER |= TIM_CCER_CC1E;                                 // CC1E Capture enabled                             (RM 365p)
   TIMx->CCR1  = (TIMx->ARR)/2;                                     // duty ratio 50% 
   
  // Enable TIMx 
  TIMx->CR1 |= TIM_CR1_CEN;                                        //counter enable

// 2. ADC HW Trigger Config.
   // Select Trigger Source           
   ADC1->CR2 &= ~ADC_CR2_JEXTSEL;                                  // reset EXTSEL
   if(timer == 1)      ADC1->CR2 |= 1<<16;   // TIMx TRGO event 
   else if(timer == 2) ADC1->CR2 |= 3<<16; 
   else if(timer == 4) ADC1->CR2 |= 9<<16; 
   else if(timer == 5) ADC1->CR2 |= 11<<16; 
   
   
   //Select Trigger Polarity
   ADC1->CR2 &= ~ADC_CR2_JEXTEN;                                      // reset EXTEN, default
   if(edge==RISE)      ADC1->CR2 |= ADC_CR2_JEXTEN_0;      // trigger detection rising edge
   else if(edge==FALL) ADC1->CR2 |= ADC_CR2_JEXTEN_1;      // trigger detection falling edge
   else if(edge==BOTH) ADC1->CR2 |= ADC_CR2_JEXTEN_Msk;   // trigger detection both edge
}



void ADC_JSW_start(ADC_TypeDef *ADCx){
   // Enable ADON, SW Trigger-------------------------------------------------------------------------------
   ADCx->CR2 |= ADC_CR2_ADON;
   ADCx->CR2 |= ADC_CR2_JSWSTART;
}


uint32_t ADC_Jread(){
   return ADC1->DR;
}

uint32_t is_ADC_JEOC(ADC_TypeDef *ADCx){
   return (ADCx->SR & ADC_SR_JEOC) == ADC_SR_JEOC;
}

void ADC1_JEOC_Enable(void){            // JEOC Interrupt Enable
   ADC1->CR1 &= ~ADC_CR1_JEOCIE;          // JEOC Interrupt reset
   ADC1->CR1 |= ADC_CR1_JEOCIE;           // JEOC Interrupt enable
}


void ADC_Injection_sequence(int length, int *seq){
   
   ADC1->JSQR &= ~(3<<20);                   // reset length of conversions in the regular channel    
   ADC1->JSQR |= (length-1)<<20;             // conversions in the regular channel conversion sequence
   
   if(length>1) ADC1->CR1 |= ADC_CR1_SCAN;  // 1: Scan mode enable
   
   if (length == 1){
      ADC1->JSQR &=~(0x1F<<15);
      ADC1->JSQR |=seq[0]<<15;
   }
   else if(length == 2){
      ADC1->JSQR &=~(0x1F<<10);
      ADC1->JSQR |=seq[0]<<10;
      
      ADC1->JSQR &=~(0x1F<<15);
      ADC1->JSQR |=seq[1]<<15;
   }
   else if(length == 3){
      ADC1->JSQR &=~(0x1F<<5);
      ADC1->JSQR |=seq[0]<<5;
      
      ADC1->JSQR &=~(0x1F<<10);
      ADC1->JSQR |=seq[1]<<10;
      
      ADC1->JSQR &=~(0x1F<<15);
      ADC1->JSQR |=seq[2]<<15;
   }
   else if(length == 4){
      ADC1->JSQR &=~(0x1F<<0);
      ADC1->JSQR |=seq[0]<<0;
      
      ADC1->JSQR &=~(0x1F<<5);
      ADC1->JSQR |=seq[1]<<5;
      
      ADC1->JSQR &=~(0x1F<<10);
      ADC1->JSQR |=seq[2]<<10;
      
      ADC1->JSQR &=~(0x1F<<15);
      ADC1->JSQR |=seq[3]<<15;
   }
}
*/











//Main.c
/*
ADC_Jinit(GPIOB,0,TRGO,Prescaler_8);
ADC_Jinit(GPIOB,1,TRGO,Prescaler_8);

ADC_continue(SINGLE);
ADC_injection_sequence(2,seqCHn);

ADC_JSW_start(ADC1);


void ADC_IRQHandler(void) {
	if(is_ADC_OVR(ADC1)){
		clear_ADC_OVR(ADC1);
	}
	if(is_ADC_JEOC(ADC1){
		Result_3 = ADC1->JDR3;
		Result_4 = ADC1->JDR4;
	}
}
*/