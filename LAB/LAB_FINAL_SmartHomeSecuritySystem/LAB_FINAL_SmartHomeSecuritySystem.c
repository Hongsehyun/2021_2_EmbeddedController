/**
******************************************************************************
* @author  SSSLAB
* @Mod       2021-12-07 by SH HONG
* @brief   Embedded Controller:  FINAL PROJECT - Smart Home Security System
*
******************************************************************************
*/
#include "EC_HAL.h"
#include "string.h"

/* ====================            Pin Configuration             ==================== */
/* ====================            Pin Configuration             ==================== */ 
// USART1 = MCU to Server :: Blue-tooth (Data Log-PLX-DAQ excel) // PA9   PA10
// USART2 = MCU to PC(tera-term)                                 // PA2   PA3
// USART6 = MCU to ZIGBEE                                        // PA11  PA12

// Ultra-Sonic-Distance-Sensor                                   // PA6   PB10   TIM1_CH1[PWM_trig]   TIM2_CH3[Input Capture]
// IR Sensor                                                     // PB1   
// PIR Sensor                                                    // PB8
// Sound Sensor                                                  // PB0
// Light Sensor                                                  // PC0
                      
// Timer 2 = Ultrasonic Distance Sensor Input Capture Timer
// Timer 3 = ADC, Ultrasonic Distance Sensor PMM Trigger Signal                         
// Timer 4 = Timer Delay :: Light Sensor :: Curtain Control      // PB7


/* ====================        PIR parameter Configuration       ==================== */
/* ====================        PIR parameter Configuration       ==================== */ 
#define PIR_PIN  8   // PB8


/* ====================         Interrupt Configuration          ==================== */
/* ====================         Interrupt Configuration          ==================== */ 
void EXTI0_IRQHandler(void);
void EXTI9_5_IRQHandler(void);
void EXTI15_10_IRQHandler(void);

void TIM5_IRQHandler(void);
void TIM4_IRQHandler(void);
void TIM2_IRQHandler(void);

void ADC_IRQHandler(void);

void USART1_IRQHandler(void);
void USART2_IRQHandler(void);
void USART6_IRQHandler(void);

/* ====================          Function Configuration          ==================== */
/* ====================          Function Configuration          ==================== */ 
void setup(void);
void sendBuffer(char mode, char flag, char command);
void Flag_Reset(void);
void DAQ_print(void);


/* ====================      USART Variables Configuration       ==================== */
/* ====================      USART Variables Configuration       ==================== */ 
static volatile uint8_t mcu2Data     = 0;
static volatile int     indx         = 0;
static volatile int     maxBuf       = 10;
static volatile uint8_t modebuf[4]   = {'N','X','0','*'};
static volatile uint8_t getSIREN[4] = {'0','*','*','*'};
static uint8_t buf1[4];
static uint8_t buf2[4];
static uint8_t buf3[4];
static uint8_t buf4[4];

/* ====================               Sensor Value               ==================== */
/* ====================               Sensor Value               ==================== */ 
static volatile int  reflect = 0;
static volatile int  sound   = 0;
static volatile int  dist    = 0;

/* ==================== UltraSonic Sensor Variable Configuration ==================== */
/* ==================== UltraSonic Sensor Variable Configuration ==================== */ 
static volatile uint32_t capture_start = 0;
static volatile uint32_t ovf_cnt       = 0;
static volatile float timeInterval     = 0;
static volatile float timeSt           = 0;
static volatile float timeEnd          = 0;
static volatile float PulseWidth       = 10;   // 10us

static volatile int Total_dist_cnt = 0;
static volatile int Sub_dist_cnt = 0;

/* ====================   ADC[IR,SOUND] parameter Configuration  ==================== */
/* ====================   ADC[IR,SOUND] parameter Configuration  ==================== */ 
static int seqCHn[2]                 = {9,8};
static volatile int FLAG_ADC         = 0;
//PB1 9     //PB0 8     //PC0 10

/* ====================    FLAG & Threshold Value Configuration  ==================== */
/* ====================    FLAG & Threshold Value Configuration  ==================== */ 
typedef struct {
   uint32_t PERSON_OUTSIDE;       // UltraSonic Distance Sensor
   uint32_t PERSON_INSIDE;        // PIR Sensor
   uint32_t WIN_OPEN;             // IR Sensor
   uint32_t WIN_BRAKE;            // SOUND Sensor
   uint32_t CURTAIN_CLOSE;        // LIGHT Sensor
   uint32_t SIREN_TRG;            // SIREN Trigger
   uint32_t SIREN_ON;             // SIREN ON
   uint32_t MODE;                 // MODE(0) = Normal     MODE(1) = SECURITY
	 uint32_t PIR;
} FLAG_STATUS;
static volatile FLAG_STATUS Flag;

typedef struct {
   uint32_t WIN_OPEN;             // IR Sensor
   uint32_t WIN_BREAK;            // SOUND Sensor
	 uint32_t Light;
} curVal;
static volatile curVal cur;

typedef struct {
   uint32_t PERSON_OUTSIDE;        // UltraSonic Distance Sensor
   uint32_t WIN_OPEN;              // IR Sensor
   uint32_t WIN_BREAK;             // SOUND Sensor
	 uint32_t Light;
} preVal;
static volatile preVal prev;

typedef struct {
   uint32_t WIN_OPEN;              // IR Sensor
   uint32_t WIN_BREAK;             // SOUND Sensor
} printZIGBEE;
static volatile printZIGBEE printUSART;

// Threshold Value
static volatile int thres_distance = 30;   //[cm]
static volatile int thres_reflect  = 1000;
static volatile int thres_sound    = 1000;

/* ==================== Others ==================== */
/* ==================== Others ==================== */ 
static volatile int TIM4_PLX_cnt     = 0;
static volatile int TIM5_light_cnt   = 0;
static volatile int VISITOR_LOG      = 0;


/* ====================                        MAIN                        ==================== */ 
/* ====================                        MAIN                        ==================== */ 
void main(void){
   /* ====================                   Initialize                    ==================== */ 
   /* ====================                   Initialize                    ==================== */ 
   setup();
	 Flag_Reset();
	
   /* ====================     Inifinite Loop [polling logic goes here]    ==================== */ 
   /* ====================     Inifinite Loop [polling logic goes here]    ==================== */ 
   while(1){

   /* ====================  TIMER Interrupt :: Print every 2sec in Server  ==================== */ 
   /* ====================  TIMER Interrupt :: Print every 2sec in Server  ==================== */ 
     if(TIM4_PLX_cnt>19){
	   	 DAQ_print();
	   	 TIM4_PLX_cnt = 0;
	   }
		 
   /* ====================        IR Sensor - Window Open/Close Flag       ==================== */ 
   /* ====================        IR Sensor - Window Open/Close Flag       ==================== */ 
     if(printUSART.WIN_OPEN==1 && cur.WIN_OPEN==1){
			 sendBuffer(modebuf[0], 'W', '1');
		   printUSART.WIN_OPEN=0;
			 if(modebuf[0]=='S'){
				 modebuf[0] = 'X';
				 sendBuffer(modebuf[0], modebuf[1], modebuf[2]);
			 }
		 }
		 //else if(printUSART.WIN_OPEN==1 && cur.WIN_OPEN==0){
		 //  sendBuffer(modebuf[0], 'W', '0');
		 //   printUSART.WIN_OPEN=0;
		 //}
	
   /* ====================        SOUND Sensor - Detect Window Break       ==================== */ 
   /* ====================        SOUND Sensor - Detect Window Break       ==================== */ 
		 if(printUSART.WIN_BREAK==1 && cur.WIN_BREAK==1){
			 sendBuffer(modebuf[0], 'S', '1');
			 printUSART.WIN_BREAK=0;
			 if(modebuf[0]=='S'){
				 modebuf[0] = 'X';
				 sendBuffer(modebuf[0], modebuf[1], modebuf[2]);
			 }
		 }
		 //else if(printUSART.WIN_BREAK==1 && cur.WIN_BREAK==0){
		 //  sendBuffer(modebuf[0], 'S', '0');
		 //   printUSART.WIN_BREAK=0;
		 //}

		 delay_ms(500);
	 }
}
/* ====================                     Set UP                       ==================== */ 
/* ====================                     Set UP                       ==================== */ 
void setup(){ 
/* ======================              CLK Configuration               ====================== */  
/* ======================              CLK Configuration               ====================== */  
  RCC_PLL_init();
  SysTick_init(1);

/* ======================          EXTI & GPIO Configuration           ====================== */  
/* ======================          EXTI & GPIO Configuration           ====================== */  
  GPIO_init(GPIOC,0,INPUT);
  // EXTI Configuration :: Button Push = Mode Change
  EXTI_init(GPIOC, BUTTON_PIN, FALL, 3);
  // EXTI Configuration :: PIR
  EXTI_init(GPIOB, PIR_PIN, RISE, 3);
  // EXTI Configuration :: LIGHT
  EXTI_init(GPIOC, 0, RISE, 3);
   
/* ==================     PWM Ultra Sonic Distance Sensor Configuration     ================== */  
/* ==================     PWM Ultra Sonic Distance Sensor Configuration     ================== */  
  PWM_t Ultrasonic_trig;                             // PWM1 for trig
  PWM_init(&Ultrasonic_trig, GPIOA, 6);              // PWM init as PA_6: Ultrasonic trig pulse
  PWM_period_us_SmartHome(&Ultrasonic_trig,100000);  // PWM of 100ms period. Use period_us()
  PWM_pulsewidth_us(&Ultrasonic_trig,10);            // PWM pulse width of 10us
	
/* =======================          Input Capture Configuration        ======================= */ 
/* =======================          Input Capture Configuration        ======================= */ 
  IC_t echo;                                              // Input Capture for echo
  ICAP_init(&echo, GPIOB, 10);                            // ICAP init as PB10 as input caputre
  ICAP_counter_us(&echo, 10);                             // ICAP counter step time as 10us
  ICAP_setup(&echo, 3, RISE);                             // TIM2_CH3 as IC3 , rising edge detect
  ICAP_setup(&echo, 4, FALL);                             // TIM2_CH3 as IC4 , falling edge detect
	
/* =======================             Enable TIMx interrupt           ======================= */ 
/* =======================             Enable TIMx interrupt           ======================= */ 
	TIM_INT_enable(TIM2);           
  TIM_INT_init(TIM5, 100);
	TIM_INT_enable(TIM5);           
  TIM_INT_init(TIM4, 100);
	TIM_INT_enable(TIM4);
	
/* =======================             USART Configuration             ======================= */ 
/* =======================             USART Configuration             ======================= */ 
  // USART init Configuration   --------------------------------------------------------------   
  USART_begin(USART1, GPIOA, 9, GPIOA, 10, 9600);              // Bluetooth    TX: PA9   RX: PA10
  USART_init(USART2, 9600);                                    //              TX: PA2   RX: PA3
  USART_init(USART6, 9600);                                    // ZIGBEE       TX: PA11  RX: PA12
	
/* =======================              ADC Configuraion               ======================= */ 
/* =======================              ADC Configuraion               ======================= */ 
  ADC_init(GPIOB,1,TRGO, 8);                                                           // ADC_IN9
  ADC_init(GPIOB,0,TRGO, 8);                                                           // ADC_IN8
   
  ADC_TRGO(TIM3, 100, RISE);
  ADC_sequence(2,seqCHn);
  ADC_continue(SINGLE);
  ADC_start(ADC1);

/* =================  Setting Time (Wait for Input Capture) = about 6~7 sec  ================= */ 
/* =================  Setting Time (Wait for Input Capture) = about 6~7 sec  ================= */ 
  printf("Getting ready to run. Please wait a moment.\r\n");              delay_ms(7777);

/* ====================           USART1 excel_DAQ initialize             ==================== */ 
/* ====================           USART1 excel_DAQ initialize             ==================== */ 
  USART_write(USART1,(unsigned char*) "CLEARSHEET\r\n",12);   
  USART_write(USART1,(unsigned char*) "LABEL,Date,Time,Timer,SIREN,VISITOR_LOG,Sound,Dist\r\n",52);   
}
/* ====================            Send Data to MCU1(actuator)             ==================== */ 
/* ====================            Send Data to MCU1(actuator)             ==================== */ 
void sendBuffer(char mode, char flag, char command){
  modebuf[0] = mode;
	modebuf[1] = flag;
	modebuf[2] = command;
	USART_write(USART6, &modebuf, 4);
  //USART_write(USART2, &modebuf, 4);
  //USART_write(USART2, &linedown, 4);
	delay_ms(500);
}
/* ====================                     Reset Flag                     ==================== */ 
/* ====================                     Reset Flag                     ==================== */ 
void Flag_Reset(void){
	 Flag.PERSON_OUTSIDE = 0;
   Flag.PERSON_INSIDE  = 0;
	 Flag.WIN_OPEN       = 0;
	 Flag.WIN_BRAKE      = 0;
	 Flag.CURTAIN_CLOSE  = 0;
	 Flag.SIREN_TRG      = 0;
	 Flag.SIREN_ON       = 0;
	 Flag.MODE           = 0;
	 Flag.PIR            = 0;
	 
	 cur.WIN_OPEN        = 0;
	 cur.WIN_BREAK       = 0;
	 cur.Light           =0;
	
	 prev.PERSON_OUTSIDE =0;
	 prev.WIN_OPEN       =0;
	 prev.WIN_BREAK      =0;
	 prev.Light          =0;
	 
	 printUSART.WIN_BREAK=0;
	 printUSART.WIN_OPEN =0;
	 
   TIM4_PLX_cnt        = 0;
	 TIM5_light_cnt			 = 0;
}
/* ====================                    PLX-DAQ PRINT                   ==================== */ 
/* ====================                    PLX-DAQ PRINT                   ==================== */ 
void DAQ_print(void){
  /* ==================== USART1 - Communicate between MCU and Server::Blue-tooth(Data Log-PLX-DAQ excel) ==================== */ 
  /* ==================== USART1 - Communicate between MCU and Server::Blue-tooth(Data Log-PLX-DAQ excel) ==================== */ 
	// Temporary Storage
  sprintf(buf1, "%d", Flag.SIREN_ON);
  sprintf(buf2, "%d", VISITOR_LOG);
  sprintf(buf3, "%d", sound);
  sprintf(buf4, "%d", dist);
  // USART1 Trasnmit sensor value to server 
  USART_write(USART1,(unsigned char*) "DATA,DATE,TIME,TIMER,",21);    // transmit char to USART1
  USART_write(USART1,&buf1,4);
  USART_write(USART1,(unsigned char*) ",",1);                         // transmit char to USART1
  USART_write(USART1,&buf2,4);                                        
  USART_write(USART1,(unsigned char*) ",",1);                         // transmit char to USART1
  USART_write(USART1,&buf3,4);                                
  USART_write(USART1,(unsigned char*) ",",1);                         // transmit char to USART1
  USART_write(USART1,&buf4,4);                                       
  USART_write(USART1,(unsigned char*) ",AUTOSCROLL_20\r\n",16);       // transmit char to USART1
	VISITOR_LOG=0;
}
/* ====================     External Interrupt: Light Intensity Sensor     ==================== */ 
/* ====================     External Interrupt: Light Intensity Sensor     ==================== */ 
void EXTI0_IRQHandler(void){
  if (is_pending_EXTI(0)) {
    cur.Light = GPIO_read(GPIOC,0);
	  }
	clear_pending_EXTI(0);
}
/* ====================           External Interrupt: PIR Sensor           ==================== */ 
/* ====================           External Interrupt: PIR Sensor           ==================== */ 
void EXTI9_5_IRQHandler(void) {
    if (is_pending_EXTI(PIR_PIN)) {
      //Flag.PERSON_INSIDE = 1;
			//sendBuffer(modebuf[0], 'I', '1'); 
			Flag.PIR++;
			 if(Flag.PIR == 1) {
          Flag.PERSON_INSIDE = 1;
				  sendBuffer(modebuf[0], 'I', '1');
				  if(modebuf[0]=='S'){
				    modebuf[0] = 'X';
						sendBuffer(modebuf[0], modebuf[1], modebuf[2]);
			   }
       }
       else if(Flag.PIR == 2) {
          Flag.PERSON_INSIDE = 0;
          Flag.PIR = 0;
       }
   }
	 clear_pending_EXTI(PIR_PIN);
}
/* ====================     External Interrupt: When Button is Pressed     ==================== */ 
/* ====================     External Interrupt: When Button is Pressed     ==================== */ 
void EXTI15_10_IRQHandler(void) {
    if (is_pending_EXTI(BUTTON_PIN)) {
       if(modebuf[0]=='N' || modebuf[0]=='S') Flag.MODE =! Flag.MODE;
       else Flag.MODE = 1;  // When the button is pressed in siren mode, it goes to Secur mode
       
       if(Flag.MODE==0) {
				 //modebuf[0]= 'N';
				 sendBuffer('N',&modebuf[1], &modebuf[2]);
			 }
       if(Flag.MODE==1){
				 //modebuf[0]= 'S';
				 sendBuffer('S',&modebuf[1], &modebuf[2]);
       } 
   clear_pending_EXTI(BUTTON_PIN);
	 }
}  
/* ====================                  TIMER[TIM5] DELAY                 ==================== */ 
/* ====================                  TIMER[TIM5] DELAY                 ==================== */ 
void TIM5_IRQHandler(void){
	 if(is_UIF(TIM5)){                     // Update interrupt
   
	 if(cur.Light==1){
	   if(cur.Light != prev.Light){
	     TIM5_light_cnt++;
	   }
	   
	   if(cur.Light == prev.Light){
	     TIM5_light_cnt=0;
	   }
	   	 
	   if(TIM5_light_cnt>19){
	    	sendBuffer(modebuf[0], 'C', '1');   // Curtain Close
	    	prev.Light = cur.Light;
	    	TIM5_light_cnt = 0;
		 }
	 }
	 
	 if(cur.Light==0){
	   if(cur.Light != prev.Light){
	     TIM5_light_cnt++;
	   }
	   
	   if(cur.Light == prev.Light){
	     TIM5_light_cnt=0;
	   }
	   	 
	   if(TIM5_light_cnt>19){
	    	sendBuffer(modebuf[0], 'C', '0');   // Curtain Close
	    	prev.Light = cur.Light;
	    	TIM5_light_cnt = 0;
		 }
	 }
	 clear_UIF(TIM5);                      // clear update interrupt flag	  
   }
}
/* ====================                  TIMER[TIM4] DELAY                 ==================== */ 
/* ====================                  TIMER[TIM4] DELAY                 ==================== */ 
void TIM4_IRQHandler(void){
	 if(is_UIF(TIM4)){                     // Update interrupt
   TIM4_PLX_cnt++;
	 clear_UIF(TIM4);                      // clear update interrupt flag	  
   }
}
/* ====================     Input Capture - UltraSonic Distance Sensor     ==================== */ 
/* ====================     Input Capture - UltraSonic Distance Sensor     ==================== */ 
void TIM2_IRQHandler(void){
   if(is_UIF(TIM2)){                     // Update interrupt
     if(capture_start==1) ovf_cnt++;       // overflow count
     //TIM2_cnt++; 
		 clear_UIF(TIM2);                      // clear update interrupt flag	  
   }
   if(is_CCIF(TIM2,3)){                            // TIM2_Ch3 (IC3) Capture Flag. Rising Edge Detect
      capture_start = 1;
      timeSt = TIM2->CCR3;                     // Capture TimeStart from CC3
      clear_CCIF(TIM2,3);                 // clear capture/compare interrupt flag 
   }                 
   else if(is_CCIF(TIM2,4)){                  // TIM2_Ch4 (IC4) Capture Flag. Falling Edge Detect
      capture_start = 0;
      timeEnd = TIM2->CCR4;                     // Capture TimeEnd from CC4
      timeInterval = PulseWidth*((timeEnd-timeSt)+(0XFFFF*ovf_cnt));
      dist         = (float) timeInterval/58;   // Ultrasonic speed[m/s] * echo pulse duration[us]
		  
		  if(dist<thres_distance) Flag.PERSON_OUTSIDE = 1;
		  ovf_cnt = 0;
		 
		  Total_dist_cnt++;
      
      if (dist<30){
         Sub_dist_cnt++;   
      }
			
      if (Total_dist_cnt>10){
         if (Sub_dist_cnt>7){
            Flag.PERSON_OUTSIDE = 1;
         }
         else{
            Flag.PERSON_OUTSIDE=0;
         }
			 
				 if(Flag.PERSON_OUTSIDE!=prev.PERSON_OUTSIDE){
					 if(Flag.PERSON_OUTSIDE==0){
           sendBuffer(modebuf[0], 'O', '0');
				   }
				 
				   if(Flag.PERSON_OUTSIDE==1){
					 sendBuffer(modebuf[0], 'O', '1');
						 if(modebuf[0]=='S'){
							 VISITOR_LOG=1;
						 }
				   }
				 }
			prev.PERSON_OUTSIDE=Flag.PERSON_OUTSIDE;
      Total_dist_cnt=0;
      Sub_dist_cnt=0;
			}				 
	  clear_CCIF(TIM2,4);                          // clear capture/compare interrupt flag 
	 }
}	 
/* ====================   ADC - IR Sensor / Check if window is opened      ==================== */ 
/* ====================   ADC - Sound Sensor / Check if window is Broken   ==================== */ 
void ADC_IRQHandler(void){
   if( is_ADC_OVR() ) {       // after finishing sequence
      clear_ADC_OVR();
   } 
   if( is_ADC_EOC() ) {   
      if(FLAG_ADC==0) {
         reflect = ADC1->DR;
      }
      if(FLAG_ADC==1) {
         sound = ADC1->DR;
      }
      FLAG_ADC =! FLAG_ADC;
   }
	 
	 prev.WIN_BREAK = cur.WIN_BREAK;
   if(sound>thres_sound)      Flag.WIN_BRAKE=1;
	 else if(sound<thres_sound) Flag.WIN_BRAKE=0;       
	 cur.WIN_BREAK = Flag.WIN_BRAKE;
	 
	 if(cur.WIN_BREAK != prev.WIN_BREAK)  {
	   printUSART.WIN_BREAK = 1;
	}
	 
   prev.WIN_OPEN = cur.WIN_OPEN;
   if(reflect>thres_reflect)      Flag.WIN_OPEN=1;
	 else if(reflect<thres_reflect) Flag.WIN_OPEN=0;       
	 cur.WIN_OPEN = Flag.WIN_OPEN;
	 
	 if(cur.WIN_OPEN != prev.WIN_OPEN)  {
	   printUSART.WIN_OPEN = 1;
	}
}
/* ====================    USART6 - Communicate between MCU and ZIGBEE     ==================== */ 
/* ====================    USART6 - Communicate between MCU and ZIGBEE     ==================== */ void USART6_IRQHandler(){
   if(is_USART_RXNE(USART6)){
      mcu2Data = USART_getc(USART6);
      USART_write(USART2,&mcu2Data,1); // transmit char to USART2
      getSIREN[indx]=mcu2Data; 
      indx++;
		 
      if(mcu2Data=='*'){
        printf("\r\n");
        indx=0;
      }
			if(getSIREN[0]=='1') Flag.SIREN_ON = 1;
			if(getSIREN[0]=='0') Flag.SIREN_ON = 0;
		  if(modebuf[0]!='N' && modebuf[0]!='S' && modebuf[0]!='X' && modebuf[0]!='P'){
        indx =0;
        memset(modebuf, 0, sizeof(char) * maxBuf);       // To reset all value to 0. in buffer.
      }
   }
}
