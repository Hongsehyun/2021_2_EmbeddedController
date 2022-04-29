/**
******************************************************************************
* @author  SSSLAB
* @Mod		 2021-11-22 by SH HONG
* @brief   Embedded Controller:  USART communication
*
******************************************************************************
*/
#include "stm32f4xx.h"
#include "ecGPIO.h"
#include "ecRCC.h"
#include "ecUART.h"
#include "ecSysTick.h"
#include "String.h"

// USART2 : MCU to PC via usb 
// USART1 : MCU to MCU2

// Variables
static volatile uint8_t mcu2Data = 0;
static volatile uint8_t pcData = 0;

static volatile int 		indx =0;
static volatile int 		maxBuf=10;
static volatile uint8_t buffer[100]={0,};
static volatile int 		bReceive=0;
static volatile int		  endChar = 13;

static volatile int 		ledOn = 0;
static volatile int		  ledOn_1 = 0;
static volatile int		  ledOn_2= 0;
static volatile int		  ledOn_3 = 0;

void setup(void);
void USART1_IRQHandler(void);
void USART2_IRQHandler(void);


int main(void) {
	// Initialiization --------------------------------------------------------
	setup();
	printf("Hello Nucleo\r\n");
	
	// Inifinite Loop ----------------------------------------------------------
	while (1){
		
		/***********************   Tutorial Code   ***********************/
		/***********************   Tutorial Code   ***********************/
		//GPIO_output(GPIOA, 5, Fast, PushPull, N_PUPD);
		//
		//if(bReceive==1 && buffer[0]=='L'){
		//	printf("buffer : %s\r\n",buffer);
		//	if      (buffer[1]=='0') ledOn = 0;		
		//	else if (buffer[1]=='1') ledOn = 1;
		//	else     printf("ERROR : Wrong command\r\n");
    //
		//	bReceive = 0;
		//  GPIO_write(GPIOA, 5, ledOn);
		//}
		
		
		/***********************   LAB  Code   ***********************/
		/***********************   LAB  Code   ***********************/
		if(bReceive==1 && buffer[0]=='L') {
		  switch(buffer[1]){
        case '0': ledOn_1 = 0;  ledOn_2= 0;  ledOn_3 = 0; break;
        case '1': ledOn_1 = 0;  ledOn_2= 0;  ledOn_3 = 1; break;
        case '2': ledOn_1 = 0;  ledOn_2= 1;  ledOn_3 = 0; break;
        case '3': ledOn_1 = 0;  ledOn_2= 1;  ledOn_3 = 1; break;
        case '4': ledOn_1 = 1;  ledOn_2= 0;  ledOn_3 = 0; break;
        case '5': ledOn_1 = 1;  ledOn_2= 0;  ledOn_3 = 1; break;
        case '6': ledOn_1 = 1;  ledOn_2= 1;  ledOn_3 = 0; break;
        case '7': ledOn_1 = 1;  ledOn_2= 1;  ledOn_3 = 1;  break;
		  	default: printf("ERROR : Wrong command\r\n"); break;
      }
			bReceive = 0;	 
		}
		GPIO_write(GPIOA,6,ledOn_1);
		GPIO_write(GPIOA,7,ledOn_2);
		GPIO_write(GPIOB,6,ledOn_3);
		delay_ms(500);
	}
}


void USART1_IRQHandler(void){		      // USART1 INT 
	if(is_USART_RXNE(USART1)){          // Get only 8Bit Value
		mcu2Data = USART_getc(USART1);		// Store the input data at mcu2data valuable				
		if(mcu2Data==endChar) {           // ALL Communication END
			bReceive=1;
			indx = 0;
		}
		else{
			if(indx>maxBuf){             // if read value is over(longer) than buffer value. ERROR!
				indx =0;
				memset(buffer, 0, sizeof(char) * maxBuf);       // To reset all value to 0. in buffer.
				printf("ERROR : Too long string\r\n");
		}
	  buffer[indx] = mcu2Data;  // Write the input data in buffer. Save sequentially Like buffer[0]->buffer[1]->...
	  indx++;                   // increase buffer index sequentially.
		}
	}
}


void USART2_IRQHandler(void){		//USART2 INT 
	if(is_USART_RXNE(USART2)){    //Ready To read data
		pcData = USART_getc(USART2);    // Store the input data at pcData(1byte, 8bit valuable)
		USART_write(USART1,&pcData,1);	// transmit char to USART1
		printf("%c",pcData); 						// echo to sender(pc), Print the input data at PC screen(tera term)
		
		if(pcData==endChar){            // enter = 13 Ascii code
			printf("\r\n"); 							// to change line on PC display
		}
	}
}


// Initialiization 
void setup(void)
{
	// CLK Configuration
	RCC_PLL_init();
	SysTick_init(1);
	
	// USART congfiguration
	USART_init(USART2, 38400);  											// Communication between PC and mcu.
	USART_begin(USART1, GPIOA, 9, GPIOA, 10, 9600); 	// Communication between mcu and mcu.
	// IN USART1, we did not use the default port and pin. we use 'PA9 - RXD , PA10 - TXD'
	
	// GPIO configuration
	GPIO_init(GPIOA, 6, OUTPUT);
	GPIO_init(GPIOA, 7, OUTPUT);
	GPIO_init(GPIOB, 6, OUTPUT);
}
