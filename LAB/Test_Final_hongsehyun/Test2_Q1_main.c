/**
******************************************************************************
* @author  SSSLAB
* @Mod		 2021-12-17 by SH HONG
* @brief   Embedded Controller:  FINAL EXAM
*
******************************************************************************
*/
#include "EC_HAL.h"
#include "string.h"

/* ====================          Interrupt Configuration         ==================== */
/* ====================          Interrupt Configuration         ==================== */ 
void setup(void);

void EXTI15_10_IRQHandler(void);

void TIM5_IRQHandler(void);
void TIM4_IRQHandler(void);
void ADC_IRQHandler(void);
void USART2_IRQHandler(void);
void TIM2_IRQHandler(void);
/* ====================   ADC parameter Configuration  ==================== */
/* ====================   ADC parameter Configuration  ==================== */ 
static int seqCHn[1]                 = {9};


/* ====================   USART parameter Configuration  ==================== */
/* ====================   USART parameter Configuration  ==================== */ 
static volatile uint8_t pcData          = 0;
static volatile int       idx           = 0;
static volatile uint8_t   DATA[3]       = {'N','0','0'};
static volatile uint8_t   Buf[3]        = {'N','0','0'};
/* ==================== Others ==================== */
/* ==================== Others ==================== */ 
static volatile uint8_t LED_Blink   = 0;

static volatile uint8_t LED_state   = 0;
static volatile uint8_t LED_ON_Flag = 0;
static volatile uint8_t LED_BL_Flag = 1;


static volatile uint32_t IR_thres   = 1000;
static volatile uint32_t IR_VAL     = 0;

static volatile uint16_t TIM2_CNT   = 0;
static volatile uint16_t TIM4_CNT   = 0;
static volatile uint16_t TIM5_CNT   = 0;
static volatile uint16_t sec_2_Flag   = 0;
static volatile uint16_t stopFlag   = 0;
static volatile uint16_t runFlag   = 0;

static volatile uint8_t DIR = 0;


PWM_t FinalPWM;                              // PWM1 for trig


int main(void) { 
  /* ====================                    Initialize                    ==================== */ 
  /* ====================                    Initialize                    ==================== */ 
  setup();
	
  /* ====================      Inifinite Loop [polling logic goes here]    ==================== */ 
  /* ====================      Inifinite Loop [polling logic goes here]    ==================== */ 
  while(1){
		
		if(Buf[0]=='N'){
		//LED BLINKING
		if(LED_BL_Flag==1) GPIO_write(GPIOA,5,LED_Blink);
		if(LED_ON_Flag==1) GPIO_write(GPIOA,5,1);
		//MOTOT CONTROL
		if(stopFlag==1) PWM_duty(&FinalPWM, (float)0/200);								 // Duty 0%	
		if(runFlag==1)  PWM_duty(&FinalPWM, (float)100/200);							 // Duty 50%
		//PRITN CURRENT STATE
		}
		
		
		if(Buf[0]=='B'){
      switch(Buf[1]){
        case '1': GPIO_write(GPIOA,5,LED_Blink);
        case '0': GPIO_write(GPIOA,5,LED_Blink);
        default : break;
			}
		}
		
		
		if(Buf[0]=='F'){
      switch(Buf[1]){
        case '1': PWM_duty(&FinalPWM,(float)(100/200)); break;
        case '0': PWM_duty(&FinalPWM,(float)(1 - (float)(100/200))); break;
        default : break;
			}
		}
		 
		if(sec_2_Flag==1){
			printf("LED BLINK ON/OFF=%d, ADC=%d\r\n",LED_BL_Flag, IR_VAL);
			sec_2_Flag=0;
		}
	}
}




/* ====================                     Set UP                       ==================== */ 
/* ====================                     Set UP                       ==================== */ 
void setup(void){
/* ======================              CLK Configuration               ====================== */  
/* ======================              CLK Configuration               ====================== */  
  RCC_PLL_init();                                                        // System Clock = 84MHz
  SysTick_init(1);
	
/* ======================          EXTI & GPIO Configuration           ====================== */  
/* ======================          EXTI & GPIO Configuration           ====================== */  																		     
  GPIO_init(GPIOA,5,OUTPUT);                              //LED
  GPIO_output(GPIOA, 5, Medium, PushPull , N_PUPD);       //LED
	
	GPIO_init(GPIOC,13,INPUT);                              //BUTTON
	GPIO_pudr(GPIOC,13,PD);                                 //BUTTON
	EXTI_init(GPIOC,13,RISE,1);                             //BUTTON

	GPIO_init(GPIOA, 6, AF);																//DC MOTOR
  GPIO_init(GPIOA, 7, OUTPUT);														//DC MOTOR - DIR PIN
  GPIO_ospeed(GPIOA, 6 , High);                           //HIGH SPEED		//DC MOTOR
   
/* =======================             Enable TIMx interrupt           ======================= */ 
/* =======================             Enable TIMx interrupt           ======================= */ 																		     
  TIM_INT_init(TIM2,2000);                                             // Interrupt Period = 0.5sec     
  TIM_INT_enable(TIM2);
	TIM_INT_init(TIM4,500);                                             // Interrupt Period = 0.5sec     
  TIM_INT_enable(TIM4);
	TIM_INT_init(TIM5,1000);                                            // Interrupt Period = 1sec     
  TIM_INT_enable(TIM5);

/* ==================                  PWM Configuration                    ================== */  
/* ==================                  PWM Configuration                    ================== */  
	PWM_init(&FinalPWM, GPIOA, 6);               // PWM init as PA_6
  PWM_period_us(&FinalPWM, 200);               // PWM of 5kHz
	PWM_duty(&FinalPWM, (float)0.5);								 // Duty 50%	   - RUN DC MOTOR
	
/* =======================             USART Configuration             ======================= */ 
/* =======================             USART Configuration             ======================= */ 
  USART_init(USART2, 9600);                                          // TXD: PA2  RXD: PA3

/* =======================              ADC Configuraion               ======================= */ 
/* =======================              ADC Configuraion               ======================= */ 
  ADC_init(GPIOB,1,TRGO, 8);                                                           // ADC_IN9
  //ADC_init(GPIOB,0,TRGO, 8);                                                           // ADC_IN8

  ADC_TRGO(TIM3, 100, RISE);
  ADC_sequence(1,seqCHn);
  ADC_continue(CONT);
  ADC_start(ADC1);
}
/* ====================                  TIMER[TIM4] DELAY                 ==================== */ 
/* ====================                  TIMER[TIM4] DELAY                 ==================== */ 
void TIM4_IRQHandler(void){
	 if(is_UIF(TIM4)){                     // Update interrupt
     if(Buf[0]=='N')		 						LED_Blink =! LED_Blink;
		 if(Buf[0]=='B' && Buf[1]=='0')	LED_Blink =! LED_Blink;	 
	 clear_UIF(TIM4);                      // clear update interrupt flag	  
	 }
}
/* ====================                  TIMER[TIM2] DELAY                 ==================== */ 
/* ====================                  TIMER[TIM2] DELAY                 ==================== */ 
void TIM2_IRQHandler(void){
	 if(is_UIF(TIM2)){                     // Update interrupt
		 if(Buf[0]=='B' && Buf[1]=='1')	 LED_Blink =! LED_Blink;
	}
	clear_UIF(TIM2);                      // clear update interrupt flag	  
}
/* ====================                  TIMER[TIM5] DELAY                 ==================== */ 
/* ====================                  TIMER[TIM5] DELAY                 ==================== */ 
void TIM5_IRQHandler(void){
	 if(is_UIF(TIM5)){                     // Update interrupt
     TIM5_CNT++;
		 if(TIM5_CNT==2){
			 sec_2_Flag = 1;
			 TIM5_CNT   = 0;
		 }
	 clear_UIF(TIM5);                      // clear update interrupt flag	  
   }
}
/* ====================     External Interrupt: When Button is Pressed     ==================== */ 
/* ====================     External Interrupt: When Button is Pressed     ==================== */ 
void EXTI15_10_IRQHandler(void) {
    if(is_pending_EXTI(BUTTON_PIN)) {
			LED_state++;
			if(LED_state%2 == 1){
				LED_ON_Flag = 1;
				LED_BL_Flag = 0;
			}
			if(LED_state%2 == 0){
				LED_ON_Flag = 0;
				LED_BL_Flag = 1;
				LED_state = 0;
			}
   clear_pending_EXTI(BUTTON_PIN);
	 }
}
/* ====================   ADC - IR Sensor    ==================== */ 
/* ====================   ADC - IR Sensor    ==================== */ 
void ADC_IRQHandler(void){
   if( is_ADC_OVR() ) {       // after finishing sequence
      clear_ADC_OVR();
   } 
   if( is_ADC_EOC() ) {   
      IR_VAL = ADC1->DR;
		  
		  if(IR_VAL>IR_thres){
			  stopFlag=1;
				runFlag=0;  
		  }
			if(IR_VAL<IR_thres){
				runFlag =1;
				stopFlag=0;
			}
	 }
}
/* ====================   USART    ==================== */ 
/* ====================   USART    ==================== */ 
void USART2_IRQHandler(){		//USART2 INT 
	if(is_USART_RXNE(USART2)){
		pcData = USART_getc(USART2);
		USART_write(USART2,&pcData,1);	// transmit char to USART1
		DATA[idx] = pcData;
		if(DATA[0]=='B' || DATA[0]=='F'){
		  Buf[idx] = pcData;
		}
		idx++;
		if(idx>3) idx = 0;
		if(pcData=='*'){
			printf("\r\n"); 							// to change line on PC display
		}
	}
}

//static volatile uint8_t mcu2Data     = 0;
//static volatile uint8_t cmd           = 0;
//
//static volatile int       indx        = 0;
//static volatile int       maxBuf      = 10;
//static volatile uint8_t buffer[100] = {0,};
//static volatile uint32_t buf        = '\n\r';
//
//void USART2_IRQHandler(){		//USART2 INT 
//   if(is_USART_RXNE(USART2)){          // Get only 8Bit Value
//      mcu2Data = USART_getc(USART2);      // Store the input data at mcu2data valuable      
//      USART_write(USART2,&mcu2Data,1);      
//      
//      if(mcu2Data!='*'){
//         cmd=mcu2Data;
//      }         
//      if(mcu2Data=='*') {           // ALL Communication END
//         indx = 0;
//         USART_write(USART2,&buf,2);
//      }
//      
//      else{
//         if(indx>maxBuf){             // if read value is over(longer) than buffer value. ERROR!
//            indx =0;
//            memset(buffer, 0, sizeof(char) * maxBuf);       // To reset all value to 0. in buffer.
//            printf("\n\r  ERROR : Too long string  \n\r");
//      }
//     buffer[indx] = mcu2Data;  // Write the input data in buffer. Save sequentially Like buffer[0]->buffer[1]->...
//     indx++;                   // increase buffer index sequentially.
//      }
//   }
//}
