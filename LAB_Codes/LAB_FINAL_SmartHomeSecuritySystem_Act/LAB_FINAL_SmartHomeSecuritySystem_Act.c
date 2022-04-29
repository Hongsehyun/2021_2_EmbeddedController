/**
******************************************************************************
* @author  SSSLAB
* @Mod		 2021-12-07 by SH HONG
* @brief   Embedded Controller:  FINAL PROJECT - Smart Home Security System Act
*
******************************************************************************
*/
#include "EC_HAL.h"
#include "string.h"

/* ====================            Pin Configuration             ==================== */
/* ====================            Pin Configuration             ==================== */ 
// USART2 = MCU to PC(tera-term)                                 // PA2   PA3
// USART6 = MCU to ZIGBEE                                        // PA11  PA12

#define PB8 8                                                    // LED1 - for inDoor
#define PB9 9                                                    // LED2 - for OutDoor
#define PB4 4                                                    // Stepper Motor Pin(1)
#define PB5 5                                                    // Stepper Motor Pin(2)
#define PB3 3                                                    // Stepper Motor Pin(3)
#define PA10 10                                                  // Stepper Motor Pin(4)
//#define LED 5

/* ====================          Commnad Configuration           ==================== */
/* ====================          Commnad Configuration           ==================== */ 
/*
          PIR Sensor Keyword                    = I   (= Indoor)
          Ultra Sonic Distance Sensor Keyword   = O   (= Outdoor)
          
          Sound Sensor Keyword                  = S   (= Sound)
          IR Sensor Keyword                     = W   (= Window)
          Light Intensity Sensor Keyword        = C   (= Curtain)
          
          Button Keyword                        = B 
          
          
           MODE   ::   FLAG   ::   COMMAND
            N            C            1                  N : Normal Mode 
            S            I            0                  S : Secure Mode
            X            O                               X : Siren trigger Mode
            P            S	
                         W
*/


/* ====================          Interrupt Configuration         ==================== */
/* ====================          Interrupt Configuration         ==================== */ 
void EXTI15_10_IRQHandler(void);
void EXTI9_5_IRQHandler(void);

void USART1_IRQHandler(void);
void USART2_IRQHandler(void);
void USART6_IRQHandler(void);

void TIM5_IRQHandler(void);
void TIM4_IRQHandler(void);

/* ====================          Function Configuration          ==================== */
/* ====================          Function Configuration          ==================== */ 
void setup(void);
void Flag_Reset(void);

void NORM_mode(void);
void SECUR_mode(void);
void SIREN_countdown(int SIREN);
void SIREN_mode(void);
void SIREN_stop(void);
void MODE_Display(int mode);
void stepper_Curtain(int mode);

void bufferClear(void);
void hardwareTEST(void);
void sendBuffer(char buf1);

/* ====================      USART Variables Configuration       ==================== */
/* ====================      USART Variables Configuration       ==================== */ 
static volatile uint8_t mcu2Data	  = 0;
static volatile int 		indx        = 0;
static volatile int 		maxBuf      = 5;
static volatile uint8_t modebuf[4]  = {'N','X','X'};
static volatile uint8_t sendSIREN[4] = {'0','S','R','*'};
static volatile uint8_t sendCurtain[4] = {'0','C','R','*'};

/* ====================    FLAG & Threshold Value Configuration  ==================== */
/* ====================    FLAG & Threshold Value Configuration  ==================== */ 
typedef struct {
   int PERSON_OUTSIDE;    // UltraSonic Distance Sensor
   int PERSON_INSIDE;     // PIR Sensor
   int WIN_OPEN;          // IR Sensor
   int WIN_BRAKE;         // SOUND Sensor        
   int CURTAIN_CLOSE;     // LIGHT Sensor
   int SIREN_TRG;         // SIREN Trigger
   int SIREN_ON;          // SIREN ON
   int MODE;              // MODE(0) = Normal, MODE(1) = SECURITY
	 int curtainControl;
	 int TIM4_IndoorLED_cnt;
	 int STOP;
	 int C_open;
	 int C_close;
} FLAG_STATUS;
FLAG_STATUS Flag;

/* ==================== Others ==================== */
/* ==================== Others ==================== */ 
static volatile uint32_t blink_cnt = 0;
static volatile uint8_t segmentNUM = 5;
static volatile uint32_t TIM4_IndoorLED_cnt = 0;


/* ====================                        MAIN                        ==================== */ 
/* ====================                        MAIN                        ==================== */ 
int main(void) { 
  /* ====================                    Initialize                    ==================== */ 
  /* ====================                    Initialize                    ==================== */ 
  setup();
  Flag_Reset();
	Flag.C_open = 1;
	Flag.C_close= 0;
  /* ====================      Inifinite Loop [polling logic goes here]    ==================== */ 
  /* ====================      Inifinite Loop [polling logic goes here]    ==================== */ 
  while(1){
		
	if(Flag.STOP == 1 && modebuf[0] == 'S'){
     Flag.STOP =0 ;
	}		
		
	if(Flag.STOP == 0){ 
  /* ====================               Curtain Up and Down                ==================== */ 
  /* ====================               Curtain Up and Down                ==================== */ 	 
		 if(Flag.curtainControl == 1){
			 stepper_Curtain(Flag.CURTAIN_CLOSE);         // Flag.CURTAIN_CLOSE = 0 :: Curtain Up
			 Flag.curtainControl = 0;                     // Flag.CURTIAN_CLOSE = 1 :: Curtain Down
		 }  
		   
  /* ====================                   Mode Control                   ==================== */ 
  /* ====================                   Mode Control                   ==================== */ 
		 if(modebuf[0]=='N'){
       Flag.MODE=0;
       MODE_Display(Flag.MODE);
       switch(modebuf[1]){
         case 'C': if      (modebuf[2]=='0') { Flag.curtainControl=1;  Flag.CURTAIN_CLOSE=0; }
                   else if (modebuf[2]=='1') { Flag.curtainControl=1;  Flag.CURTAIN_CLOSE=1; }  break;
         case 'I': if(modebuf[2]=='1')       { Flag.PERSON_INSIDE  =1; }                        break;
         case 'O': if      (modebuf[2]=='1') { Flag.PERSON_OUTSIDE=1; }                               
				           else if (modebuf[2]=='0') { Flag.PERSON_OUTSIDE=0; }                         break;
         default : break;
			   }
		   }
		 
		 if(modebuf[0]=='S'){
       Flag.MODE=1;
       MODE_Display(Flag.MODE);
       switch(modebuf[1]){
         case 'C': if      (modebuf[2]=='0') { Flag.curtainControl=1;  Flag.CURTAIN_CLOSE=0; }
                   else if (modebuf[2]=='1') { Flag.curtainControl=1;  Flag.CURTAIN_CLOSE=1; } break;
         case 'I': if      (modebuf[2]=='1') { Flag.PERSON_INSIDE =1; }                        break;
         case 'O': if      (modebuf[2]=='1') { Flag.PERSON_OUTSIDE=1; }                               
				           else if (modebuf[2]=='0') { Flag.PERSON_OUTSIDE=0; }                        break;
         case 'S': if(modebuf[2]=='0') { Flag.WIN_BRAKE=0; }                                   break;
         case 'W': if(modebuf[2]=='0') { Flag.WIN_OPEN=0; }                                    break;
         default : break;
	  		 }
	  	 }
   
     if(modebuf[0]=='X'){
       switch(modebuf[1]){
         case 'I': if(modebuf[2]=='1') { Flag.PERSON_INSIDE=1;   Flag.SIREN_TRG=1;  }          break;
         case 'S': if(modebuf[2]=='1') { Flag.WIN_BRAKE=1;       Flag.SIREN_TRG=1;  }          break;
         case 'W': if(modebuf[2]=='1') { Flag.WIN_OPEN=1;        Flag.SIREN_TRG=1;  }          break;
         default : break;
			   }
		   }
	  }
		bufferClear();
	  delay_ms(500);
	}
}

/* ====================                       Set UP                       ==================== */ 
/* ====================                       Set UP                       ==================== */ 
void setup(void){
/* ======================              CLK Configuration               ====================== */  
/* ======================              CLK Configuration               ====================== */  
  RCC_PLL_init();                                                        // System Clock = 84MHz
  SysTick_us_init(1);
	
/* ======================          EXTI & GPIO Configuration           ====================== */  
/* ======================          EXTI & GPIO Configuration           ====================== */  																		     
  GPIO_init(GPIOB,PB8,OUTPUT);                                      // LED1 - for Indoor
  GPIO_init(GPIOB,PB9,OUTPUT);                                      // LED2 - for Outdoor 
	GPIO_output(GPIOB, PB8, Medium, PushPull , PU);
	GPIO_output(GPIOB, PB9, Medium, PushPull , PU);
	
	EXTI_init(GPIOC,BUTTON_PIN,FALL,3);                               // External Interrupt Setting

/* =======================  Sevensegment & Stepper Motor Configuration ======================= */ 
/* =======================  Sevensegment & Stepper Motor Configuration ======================= */ 
  sevensegment_init(5,6,7,6,7,9,8,10);                  
  Stepper_init(GPIOB,4,GPIOB,5,GPIOB,3,GPIOA,10);              // Stepper GPIO pin initialization
  Stepper_setSpeed(12, FULL);                                  //  set stepper motor speed

/* =======================             Enable TIMx interrupt           ======================= */ 
/* =======================             Enable TIMx interrupt           ======================= */ 																		     
  TIM_INT_init(TIM5,1000);                                             // Interrupt Period = 1sec     
  TIM_INT_enable(TIM5);                                
	TIM_INT_init(TIM4,100);                                             // Interrupt Period = 1sec     
  TIM_INT_enable(TIM4);                                
	
/* =======================             USART Configuration             ======================= */ 
/* =======================             USART Configuration             ======================= */ 
  USART_init(USART2, 9600);                                          // TXD: PA2  RXD: PA3
  USART_begin(USART6, GPIOA, 11, GPIOA, 12, 9600);                   // USART6 TXD: PA11 RXD: PA12
}
/* ====================                     Reset Flag                     ==================== */ 
/* ====================                     Reset Flag                     ==================== */ 
void Flag_Reset(void){
 	 Flag.PERSON_OUTSIDE   = 0;                                // UltraSonic Distance Sensor
   Flag.PERSON_INSIDE    = 0;                                // PIR Sensor
   Flag.WIN_OPEN         = 0;                                // IR Sensor
   Flag.WIN_BRAKE        = 0;                                // SOUND Sensor        
   Flag.CURTAIN_CLOSE    = 0;                                // LIGHT Sensor
   Flag.SIREN_TRG        = 0;                                // SIREN Trigger
   Flag.SIREN_ON         = 0;                                // SIREN ON
   Flag.MODE             = 0;                                // MODE(0) = Normal, MODE(1) = SECURITY
	 Flag.curtainControl   = 0;
 	 Flag.TIM4_IndoorLED_cnt = 0;
}
/* ====================                    CLEAR BUFFER                    ==================== */ 
/* ====================                    CLEAR BUFFER                    ==================== */ 
void bufferClear(void){
	modebuf[1] = ' ';
	modebuf[2] = ' ';
}
/* ====================               Send Data to Sensor MCU              ==================== */ 
/* ====================               Send Data to Sensor MCU              ==================== */ 
void sendBuffer(char buf1){
	sendSIREN[0] = buf1;
	USART_write(USART6, &sendSIREN, 4);
	delay_ms(500);
}
/* ====================       InDoor and OutDoor LED Configuration         ==================== */ 
/* ====================       InDoor and OutDoor LED Configuration         ==================== */ 
void TIM4_IRQHandler(void){
   if(is_UIF(TIM4)){
     /* ===============     PERSON_INSIDE Flag = 1   ::   INdoor LED ON    ==================== */ 
     /* ===============     PERSON_INSIDE Flag = 1   ::   INdoor LED ON    ==================== */ 
		 if(Flag.PERSON_INSIDE==1 && Flag.MODE==0){                                    // Normal State
			 GPIO_write(GPIOB, 8, HIGH);
			 Flag.TIM4_IndoorLED_cnt=1;
			 Flag.PERSON_INSIDE=0;
		 }
		 
		 if(Flag.TIM4_IndoorLED_cnt ==1){
			 TIM4_IndoorLED_cnt++;
			 if(TIM4_IndoorLED_cnt>30){
				 GPIO_write(GPIOB, 8, LOW);
			   Flag.TIM4_IndoorLED_cnt =0;
				 TIM4_IndoorLED_cnt = 0;
			 }
		 }
		 
		 if(Flag.PERSON_INSIDE==1 && Flag.MODE==1){                                     // SECUR State
			 GPIO_write(GPIOB, 8, HIGH);
			 Flag.PERSON_INSIDE = 0;
		 }
		 
		 /* ===============     PERSON_OUTSIDE Flag = 1  ::  OUTdoor LED ON     ================== */ 
     /* ===============     PERSON_OUTSIDE Flag = 1  ::  OUTdoor LED ON     ================== */ 
		 if(Flag.PERSON_OUTSIDE==1){
		   GPIO_write(GPIOB, 9, HIGH);
		 }
		 if(Flag.PERSON_OUTSIDE==0){ 
		   GPIO_write(GPIOB, 9, LOW);
		 }
	 }
	 clear_UIF(TIM4); 
}
/* ====================      SIREN COUNTDOWN and BLINK configuration       ==================== */ 
/* ====================      SIREN COUNTDOWN and BLINK configuration       ==================== */ 
void TIM5_IRQHandler(void){
   if(is_UIF(TIM5)){
     if(Flag.STOP == 0){
      // SIREN COUNTDOWN(SIREN Trigged)
      if(modebuf[0]=='X' && segmentNUM!=0 && Flag.SIREN_TRG==1){
        segmentNUM--;
		  
		  	switch(segmentNUM){
          case 4: sevensegment_decode(4); break;    
          case 3: sevensegment_decode(3); break;
          case 2: sevensegment_decode(2); break;
          case 1: sevensegment_decode(1); break;
	        case 0: sevensegment_decode(0); break;
          default : break;
			  }
			}				
				
			if(modebuf[0]=='X' && segmentNUM==0 && Flag.SIREN_TRG ==1){
				Flag.SIREN_ON=1;
		    Flag.curtainControl = 1;
				Flag.CURTAIN_CLOSE  = 0;
				Flag.SIREN_TRG  =0;
			}
			
			// SIREN ON
			if(Flag.SIREN_ON==1){  
			  blink_cnt++;
				sendBuffer('1');
			  if( blink_cnt%2 == 1 ){
          sevensegment_decode(10); 
				}
				if( blink_cnt%2 == 0 ){
          sevensegment_decode(0); 
				}
			}
	 }
 }
	 clear_UIF(TIM5); 
}
/* ====================     External Interrupt: When Button is Pressed     ==================== */ 
/* ====================     External Interrupt: When Button is Pressed     ==================== */ 
void EXTI15_10_IRQHandler(void) {
   if (is_pending_EXTI(BUTTON_PIN)) {
      SIREN_stop();
      clear_pending_EXTI(BUTTON_PIN); // cleared by writing '1'
   }
}
/* ====================          Display Mode State at 7-segment           ==================== */
/* ====================          Display Mode State at 7-segment           ==================== */ 
void MODE_Display(int mode) {
   switch(mode){
      case 0: sevensegment_decode(1); break; // display "1" when NORM_MODE
      case 1: sevensegment_decode(5); break; // display "5" when SECUR_MODE
      default : break;
   }
}
/* ====================                   Curtain Control                  ==================== */
/* ====================                   Curtain Control                  ==================== */
void stepper_Curtain(int cmd_1) {   // cmd_1 = Curtain Close Command
   if(cmd_1==1 && Flag.C_close==0) {
		 Stepper_step(2048, 0, FULL);		 // Clock-wise Rotate  :: Curtain Close
     Flag.C_close = 1;
     Flag.C_open = 0; 
		 //USART_write(USART6,sendCurtain,4);
	 }
   if(cmd_1==0 && Flag.C_open==0) {
		 Stepper_step(2048, 1, FULL);                                    // Counter-Clock-wise Rotate  :: Curtain Open
	   Flag.C_open = 1;
		 Flag.C_close = 0;
		 //USART_write(USART6,sendCurtain,4);
	 }
}
/* ====================              Stop State Configuration              ==================== */
/* ====================              Stop State Configuration              ==================== */
void SIREN_stop() {
	 segmentNUM = 5;
   modebuf[0] = 'P';
   sevensegment_decode(segmentNUM);
	  
	 Flag.STOP = 1;
	 Flag_Reset();
	 sendBuffer('0');
	 
	 GPIO_write(GPIOB,8,LOW);
	 GPIO_write(GPIOB,9,LOW);
}
/* ====================    USART6 - Communicate between MCU and ZIGBEE     ==================== */ 
/* ====================    USART6 - Communicate between MCU and ZIGBEE     ==================== */ 
void USART6_IRQHandler(){
   if(is_USART_RXNE(USART6)){
      mcu2Data = USART_getc(USART6);
      USART_write(USART2,&mcu2Data,1); // transmit char to USART2
      modebuf[indx]=mcu2Data;
      indx++;
      if(mcu2Data=='*'){
        printf("\r\n");
        indx=0;
			if(modebuf[0]!='N' && modebuf[0]!='S' && modebuf[0]!='X' && modebuf[0]!='P'){
        indx =0;
        memset(modebuf, 0, sizeof(char) * maxBuf);       // To reset all value to 0. in buffer.
       }
		}
	}
}
