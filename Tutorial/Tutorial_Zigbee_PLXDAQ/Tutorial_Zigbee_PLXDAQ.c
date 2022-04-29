///**
//******************************************************************************
//* @author  SSSLAB
//* @Mod		 2021-12-03 by SH HONG
//* @brief   Embedded Controller:  USART communication
//*
//******************************************************************************
//*/
////EC_PLX_DAQ_demo.c
////EC_PLX_DAQ_demo.c
////EC_PLX_DAQ_demo.c
//
//#include "EC_HAL.h"
//#include "String.h"
//// USART2 : MCU to PC via usb 
//
//uint8_t buf1[4];
//uint8_t buf2[4];
//uint8_t buf3[4];
//uint8_t buf4[4];
//
//uint32_t reflect=0, sound=0, lux=0, dist = 0;
//int flag = 0;
//int i =0;
//
//void setup(void);
//
//int main(void) {
//	// Initialiization --------------------------------------------------------
//	setup();
//	printf("Hello Nucleo\r\n");
//	
//	//USART1 excel_DAQ initialize 
//	USART_write(USART2,(unsigned char*) "CLEARSHEET\r\n",12);	
//	USART_write(USART2,(unsigned char*) "LABEL,Date,Time,Timer,Sound,Light,Dist\r\n",40);	
//	
//	// Inifinite Loop ----------------------------------------------------------
//	while (1){
//		
//		lux = GPIO_read(GPIOA,0);
//		sound++;
//		dist++;
//		
//		sprintf(buf1, "%d", sound);
//		sprintf(buf2, "%d", lux);
//		sprintf(buf3, "%d", dist);
//		
//		//USART1 Trasnmit sensor value to server 
//		USART_write(USART2,(unsigned char*) "DATA,DATE,TIME,TIMER,",21);	// transmit char to USART6
//		USART_write(USART2,&buf1,4);
//		USART_write(USART2,(unsigned char*) ",",1);	// transmit char to USART6
//		USART_write(USART2,&buf2,4);
//		USART_write(USART2,(unsigned char*) ",",1);	// transmit char to USART6
//		USART_write(USART2,&buf3,4);
//		USART_write(USART2,(unsigned char*) ",AUTOSCROLL_20\r\n",16);	// transmit char to USART6
//		
//		delay_ms(1000);
//	}
//}
//
//
//// Initialiization 
//void setup(void)
//{
//	RCC_PLL_init();
//	SysTick_init(1);
//	
//	// USART congfiguration
//	USART_init(USART2, 9600);
//	
//	// GPIO configuration
//	// LED_init();
//	// GPIO_init(GPIOA,0,INPUT);
//}







//EC_zigbee_demo.c
//EC_zigbee_demo.c
//EC_zigbee_demo.c

// USART2 : MCU to PC via usb 
// USART1 : MCU to MCU2 via zigbee

#include "EC_HAL.h"
#include "string.h"

uint8_t mcu2Data = 0;
uint8_t pcData = 0;
int indx =0;
int maxBuf=10;
uint8_t buffer[100]={0,};
int bReceive=0;
int ledOn = 0;
int endChar = 13;
int i =0;

void setup(void);

int main(void) {
	// Initialiization --------------------------------------------------------
	setup();
	printf("Hello Nucleo\r\n");

	// Inifinite Loop ----------------------------------------------------------
	while (1){
	
		if(bReceive==1 && buffer[0]=='L'){
			printf("buffer : %s\r\n",buffer);
			if (buffer[1]=='0') ledOn = 0;		
			else if (buffer[1]=='1') ledOn = 1;
			else printf("ERROR : Wrong command\r\n");
			bReceive = 0;
		}

		GPIO_write(GPIOA,5,ledOn);
		delay_ms(1000);
	}
}

// Initialiization 
void setup(void)
{
	RCC_PLL_init();
	SysTick_init(1);
	
	// GPIO configuration
	GPIO_init(GPIOA, 5, OUTPUT);
	
	// USART congfiguration
	USART_init(USART2, 9600);
	USART_init(USART6, 9600); 	// PA9 - RXD , PA10 - TXD
}


void USART6_IRQHandler(){		//USART1 INT 
	if(is_USART_RXNE(USART6)){
		mcu2Data = USART_getc(USART6);
		//printf("%c",pcData); 						// echo to sender(pc)		
		if(mcu2Data==endChar) {
			bReceive=1;
			indx = 0;
		}
		else{
			if(indx>maxBuf){
				indx =0;
				memset(buffer, 0, sizeof(char) * maxBuf);
				printf("ERROR : Too long string\r\n");
			}
			buffer[indx] = mcu2Data;
			indx++;
		}
	}
}

void USART2_IRQHandler(){		//USART2 INT 
	if(is_USART_RXNE(USART2)){
		pcData = USART_getc(USART2);
		USART_write(USART6,&pcData,1);	// transmit char to USART1
		printf("%c",pcData); 						// echo to sender(pc)
		
		if(pcData==endChar){
			printf("\r\n"); 							// to change line on PC display
		}
	}
}
