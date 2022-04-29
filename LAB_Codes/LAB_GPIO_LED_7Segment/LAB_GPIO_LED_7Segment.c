/*
******************************************************************************
* @author	Hong Se Hyun
* @Mod		2021-10-12  	
* @brief	Embedded Controller:  LAB 7-segment 
*					 - 7 segment decoder
* 					-  PA5, PA6, PA7, PB6, PC7, PA9, PA8, PB10  for DOUT
******************************************************************************
*/

#include "stm32f4xx.h"

//#include "EC_GPIO_API.h"
#include "ecGPIO.h"
#include "ecRCC.h"

//PA5, PA6, PA7, PB6, PC7, PA9, PA8, PB10  for DOUT
#define LED_PIN1 	 5
#define LED_PIN2 	 6
#define LED_PIN3 	 7
#define LED_PIN4 	 6
#define LED_PIN5 	 7
#define LED_PIN6 	 9
#define LED_PIN7 	 8
#define LED_PIN8 	 10
#define BUTTON_PIN 13

//GPIO Push-Pull	 
#define N_PUPD		0x00
#define PU				0x01
#define PD				0x02
#define Reserved	0x03

//GPIO Speed
#define Low       0x00
#define Medium  	0x01
#define Fast			0x02
#define High    	0x03

//GPIO Output Type
#define PushPull			0x00
#define OpenDrain    	0x01

void setup(void);


int main(void) { 
	// Initialiization --------------------------------------------------------
	setup();
	
	unsigned int cnt = 1;
	int stay = 0;
	int push = 0;
	
	// Inifinite Loop ----------------------------------------------------------
	while(1){
		// button.fall
		 if(GPIO_read(GPIOC, BUTTON_PIN) == 0) {
         stay=0; // off
         push=1;
      }
    
			// button.rise 
		 else {
         stay=1; 
			}
			
			if(stay==1 && push==1) {
				sevensegment_decode(cnt % 10);
				cnt++;
				push = 0;
				if(cnt > 9)   cnt = 0;
		}
	}
}


// Initialiization 
void setup(void)
{
	//In Simul , CLK Disable must
	RCC_HSI_init();
	//Initialization Input
	GPIO_init(GPIOC, BUTTON_PIN, INPUT);   // calls RCC_GPIOC_enable()
	//Initialization Output_Segment
	sevensegment_init(LED_PIN1, LED_PIN2, LED_PIN3, LED_PIN4, LED_PIN5, LED_PIN6, LED_PIN7, LED_PIN8);
	
	//Input Setting
	GPIO_pudr(GPIOC, BUTTON_PIN, PU);
	//Setting Input and Output Mode
	GPIO_output(GPIOA, LED_PIN1, Fast, PushPull , N_PUPD);
	GPIO_output(GPIOA, LED_PIN2, Fast, PushPull , N_PUPD);
	GPIO_output(GPIOA, LED_PIN3, Fast, PushPull , N_PUPD);
	GPIO_output(GPIOB, LED_PIN4, Fast, PushPull , N_PUPD);
	GPIO_output(GPIOC, LED_PIN5, Fast, PushPull , N_PUPD);
	GPIO_output(GPIOA, LED_PIN6, Fast, PushPull , N_PUPD);
	GPIO_output(GPIOA, LED_PIN7, Fast, PushPull , N_PUPD);
	GPIO_output(GPIOB, LED_PIN8, Fast, PushPull , N_PUPD);
	
	//Initial State == 0, When Reset
	sevensegment_decode(0);
}
