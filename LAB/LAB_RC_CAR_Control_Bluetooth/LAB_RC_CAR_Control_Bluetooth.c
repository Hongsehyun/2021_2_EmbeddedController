/**
******************************************************************************
* @author  SSSLAB
* @Mod       2021-11-22 by SH HONG
* @brief   Embedded Controller:  USART communication
*
******************************************************************************
*/
#include "EC_HAL.h"

// Pin Configuration
#define PWMpin_1 0   // servo motor pin
#define PWMpin_2 6   // DC motor pin
#define Dout     4   // Motor Driver Digital Out

// Interrupt Configuration
void USART2_IRQHandler(void);
void USART1_IRQHandler(void);

// Variables Configuration
static volatile uint8_t pcData          = 0;
static volatile uint8_t mcu2Data     = 0;
static volatile uint8_t cmd           = 0;

static volatile int       indx        = 0;
static volatile int       maxBuf      = 10;
static volatile uint8_t buffer[100] = {0,};
static volatile uint32_t buf        = '\n\r';

// ASCII CODE
static volatile int        endChar     = 13;

// Initial Motor State
static volatile float    PWM1_Pulsewidth     = 1.5;     // Servo Motor
static volatile float     PWM2_Pulsewidth     = 0.0;     // DC Motor
// Control DC/RC Motor Duty Ratio
static volatile float     PWM1_period     = 20;       // period = 20ms = 0.02sec                  Freq = 50Hz
static volatile float     PWM2_period     = 30000;    // period = 8888us = 8.888ms = 0.00888sec   Freq = 115Hz
static volatile float     PWM1_interval   = 0.5;      // Control DC Motor, for 4 Steps.
static volatile float     PWM2_interval   = 7500;     // Control DC Motor, for 4 Steps.

// Default DIR Configuration
static volatile uint8_t DIR               = 2;          // STOP STATE
//static volatile char    DIRprint[3] = {"FWD", "BWD", "STOP"};

// PWM Structure
PWM_t pwm_1;   // Servo Motor
PWM_t pwm_2;   // DC Motor

void setup(void);
void RC_control(uint8_t character);
void RC_print(float duty_1, float duty_2, int delay);


int main(void) {
   // Initialiization --------------------------------------------------------
   setup();
   
   // Inifinite Loop ----------------------------------------------------------
   while (1){
      RC_print((float)(PWM1_Pulsewidth/PWM1_period), (float)(PWM2_Pulsewidth/PWM2_period), 1000);
   }
}   


void setup() {
   // CLK Configuration
   RCC_PLL_init();
   SysTick_init(1);
   
   // GPIO configuration
   GPIO_init(GPIOA, PWMpin_1, AF);
   GPIO_init(GPIOA, PWMpin_2, AF);
  GPIO_init(GPIOA, Dout , OUTPUT);
   GPIO_ospeed(GPIOA, PWMpin_1 , High);   // HIGH SPEED
   GPIO_ospeed(GPIOA, PWMpin_2 , High);   // HIGH SPEED
   
   //PWM Setting
   PWM_init(&pwm_1, GPIOA, PWMpin_1 );    // 50Hz     // Using Timer2, Ch1 
   PWM_period_ms(&pwm_1, PWM1_period);     
   PWM_init(&pwm_2, GPIOA, PWMpin_2 );    // 1kHz     // Using Timer3, Ch1
   PWM_period_us(&pwm_2, PWM2_period);
  PWM_duty(&pwm_1,PWM1_Pulsewidth/PWM1_period);
  PWM_duty(&pwm_2,PWM2_Pulsewidth/PWM2_period);   
   
   // USART congfiguration
   // IN USART1, we did not use the default port and pin. we use 'PA9 - RXD , PA10 - TXD'
   USART_init(USART2, 38400);                                   // Communication between PC and mcu.
   USART_begin(USART1, GPIOA, 9, GPIOA, 10, 9600);    // Bluetooth
   
   // Set-Up Finished
   printf("          Hello Handong          \r\n");
}


void USART1_IRQHandler(void){            // USART1 INT 
   if(is_USART_RXNE(USART1)){          // Get only 8Bit Value
      mcu2Data = USART_getc(USART1);      // Store the input data at mcu2data valuable      
      USART_write(USART1,&mcu2Data,1);      
      
      if(mcu2Data!=endChar){
         cmd=mcu2Data;
      }         
      if(mcu2Data==endChar) {           // ALL Communication END
         RC_control(cmd);
         indx = 0;
         USART_write(USART1,&buf,2);
      }
      
      else{
         if(indx>maxBuf){             // if read value is over(longer) than buffer value. ERROR!
            indx =0;
            memset(buffer, 0, sizeof(char) * maxBuf);       // To reset all value to 0. in buffer.
            printf("\n\r  ERROR : Too long string  \n\r");
      }
     buffer[indx] = mcu2Data;  // Write the input data in buffer. Save sequentially Like buffer[0]->buffer[1]->...
     indx++;                   // increase buffer index sequentially.
      }
   }
}


void USART2_IRQHandler(void){          //USART2 INT 
   if(is_USART_RXNE(USART2)){        //Ready To read data
      pcData = USART_getc(USART2);    // Store the input data at pcData(1byte, 8bit valuable)
      USART_write(USART1,&pcData,1);   // transmit char to USART1
      printf("%c",pcData);                   // echo to sender(pc), Print the input data at PC screen(tera term)

      if(pcData==endChar){            // enter = 13 Ascii code
      }
   }
}


void RC_control(uint8_t cmd){
        switch(cmd){
      case 'R': if(PWM1_Pulsewidth>0.5 && PWM1_Pulsewidth<=2.5)    PWM1_Pulsewidth-= PWM1_interval;
                        break;                              // Servo Motor Right Turn by 45degree
      case 'L': if(PWM1_Pulsewidth>=0.5 && PWM1_Pulsewidth<2.5)    PWM1_Pulsewidth+= PWM1_interval;
                        break;                              // Servo Motor Left Turn by 45degree
      case 'U': if(PWM2_Pulsewidth>=0 && PWM2_Pulsewidth<PWM2_period)   PWM2_Pulsewidth+= PWM2_interval;
                   break;                              // DC Motor Speed Up per 25%   
      case 'D': if(PWM2_Pulsewidth>0 && PWM2_Pulsewidth<=PWM2_period)   PWM2_Pulsewidth-= PWM2_interval;
                   break;                              // DC Motor Speed Down per 25%
         case 'F':   DIR=0;   break;
      case 'B': DIR=1;   break;
         case 'S': DIR=2;   break;
      default : printf("ERROR : Wrong command\r\n"); break;
    }
   //Setting DC Motor Direction
   GPIO_write(GPIOA, Dout, DIR);                    
   
   // Rotate Servo Motor
   PWM_duty(&pwm_1,(float)(PWM1_Pulsewidth/PWM1_period));
         
   // Rotate DC Motor :: Forward Direction
   if(DIR==0) PWM_duty(&pwm_2,(float)(PWM2_Pulsewidth/PWM2_period));
   // Rotate DC Motor :: Backward Direction
   if(DIR==1) PWM_duty(&pwm_2, (float)(1 - (float)PWM2_Pulsewidth/PWM2_period));
   // STOP :: GO TO initial State
   if(DIR==2){
       PWM1_Pulsewidth = 1.5;     // Servo Motor
       PWM2_Pulsewidth = 0.0;     // DC Motor
       PWM_duty(&pwm_1,PWM1_Pulsewidth/PWM1_period);
       PWM_duty(&pwm_2,PWM2_Pulsewidth/PWM2_period);
   }
}


void RC_print(float duty_1, float duty_2, int delay){
      // Rotate DC Motor :: Forward Direction
      if(DIR==0) printf("RC Car:: Servo Motor: %.1f[deg]  DC Motor Duty: %.1f [percent]  FWD\r\n", 180-(duty_1*1800-45), duty_2*100); 
      // Rotate DC Motor :: Backward Direction
      if(DIR==1) printf("RC Car:: Servo Motor: %.1f[deg]  DC Motor Duty: %.1f [percent]  BWD\r\n", 180-(duty_1*1800-45), duty_2*100); 
      // STOP :: GO TO initial State
      if(DIR==2) printf("RC Car:: Servo Motor: %.1f[deg]  DC Motor Duty: %.1f [percent]  STOP\r\n", 180-(duty_1*1800-45), duty_2*100);

      delay_ms(delay);
}
