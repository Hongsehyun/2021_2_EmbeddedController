/**
******************************************************************************
* @author	Hong Se Hyun
* @Mod		2021-10-10   	
* @brief	Embedded Controller:  LAB 7-segment 
*					 - 7 segment decoder
* 					-  PA5, PA6, PA7, PB6, PC7, PA9, PA8, PB10  for DOUT
******************************************************************************
*/

#include "stm32f4xx.h"

#include "EC_GPIO_API.h"
#include "ecGPIO.h"
#include "ecRCC.h"

//#define LED_PIN 	5
#define LED_PIN1 	 5
#define LED_PIN2 	 6
#define LED_PIN3 	 7
#define LED_PIN4 	 6
#define LED_PIN5 	 7
#define LED_PIN6 	 9
#define LED_PIN7 	 8
#define LED_PIN8 	 10
//PA5, PA6, PA7, PB6, PC7, PA9, PA8, PB10  for DOUT

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
	unsigned int cnt = 0;
	
	int stay = 0;
	int push = 0;
  int i =0;
	int state[5];
	
	
	// Inifinite Loop ----------------------------------------------------------
	while(1){
		//sevensegment_decode(cnt % 10);
		if(GPIO_read(GPIOC, BUTTON_PIN) == 0) cnt++; 
		if (cnt > 9) cnt = 0;
		for(int i = 0; i < 500000;i++){}
			
		int number[11][8]={
                    {1,1,1,0,1,1,1,0},          //zero
                    {0,0,1,0,0,1,0,0},          //one
                    {1,0,1,1,1,0,1,0},          //two
                    {1,0,1,1,0,1,1,0},          //three
                    {0,1,1,1,0,1,0,0},          //four
                    {1,1,0,1,0,1,1,0},          //five
                    {1,1,0,1,1,1,1,0},          //six
                    {1,0,1,0,0,1,0,0},          //seven
                    {1,1,1,1,1,1,1,0},          //eight
                    {1,1,1,1,0,1,1,0},          //nine
                    {0,0,0,0,0,0,0,1}          //dot
                  };
	}
}



/*
  // Inifinite Loop ----------------------------------------------------------
  while(1){
		 // button.fall
		 if(GPIO_read(GPIOC, BUTTON_PIN) == 0){
         stay=0; // off
         push=1;
      }
      
			// button.rise 
		 else {
         stay=1; 
			}
			
			if(stay==1 && push==1) {
			 	state[0] = 0;
				state[1] = 0;
				state[2] = 0;
				state[3] = 0;
				state[4] = 0;
				
				i++;
				if(i>4) {
					i=1;
				}
				
				state[i] = 1;
				
				if(state[1] == 1) {
				GPIO_write(GPIOA, LED_PIN1, HIGH);
				GPIO_write(GPIOA, LED_PIN2, LOW);
				GPIO_write(GPIOA, LED_PIN3, LOW);
				GPIO_write(GPIOB, LED_PIN4, LOW);
				push = 0;
				}
				if(state[2] == 1) {
				GPIO_write(GPIOA, LED_PIN1, LOW);
				GPIO_write(GPIOA, LED_PIN2, HIGH);
				GPIO_write(GPIOA, LED_PIN3, LOW);
				GPIO_write(GPIOB, LED_PIN4, LOW);
				push = 0;
				}
				if(state[3] == 1) {
				GPIO_write(GPIOA, LED_PIN1, LOW);
				GPIO_write(GPIOA, LED_PIN2, LOW);
				GPIO_write(GPIOA, LED_PIN3, HIGH);
				GPIO_write(GPIOB, LED_PIN4, LOW);
				push = 0;
				}
				if(state[4] == 1) {
				GPIO_write(GPIOA, LED_PIN1, LOW);
				GPIO_write(GPIOA, LED_PIN2, LOW);
				GPIO_write(GPIOA, LED_PIN3, LOW);
				GPIO_write(GPIOB, LED_PIN4, HIGH);
				push = 0;
				}
			}
	}
*/



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
}