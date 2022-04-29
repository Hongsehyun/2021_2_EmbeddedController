/**
******************************************************************************
* @author  Hong Se Hyun
* @Mod		 2021-10-03 by SHHong  	
* @brief   Embedded Controller:  LAB Digital In/Out
*					 - Toggle LED LD2 by Button B1  pressing
* 
******************************************************************************
*/

#include "stm32f4xx.h"
#include "ecGPIO.h"
#include "ecRCC.h"

#define LED_PIN 	 5
#define LED_PIN1 	 5
#define LED_PIN2 	 6
#define LED_PIN3 	 7
#define LED_PIN4 	 6

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



#include "EC_GPIO_API.h"

EC_DigitalIn button(GPIOC,BUTTON_PIN);
EC_DigitalOut led(GPIOA,LED_PIN);


void setup(void);
	
int main(void) { 

		while(1){
			if(!button)	led=1;			//if(!button.read())
			else 				led=0;
	}

	
	
/*
	// Part 1. Create EC_HAL driver
  // Initialization --------------------------------------------------------
	setup();

	// Input Setting
	GPIO_pudr(GPIOC, BUTTON_PIN, PU);
	
	// Output Setting
	GPIO_output(GPIOA, LED_PIN1, Medium, PushPull , PU);
	
	// Inifinite Loop ----------------------------------------------------------
	while(1){
		if(GPIO_read(GPIOC, BUTTON_PIN) == 0)	GPIO_write(GPIOA, LED_PIN1, HIGH);
		else 																	GPIO_write(GPIOA, LED_PIN1, LOW);
	}
*/



/*
  // Part 2. Toggle LED with push - button input
	// Initialization --------------------------------------------------------
	setup();

	// Input Setting
	GPIO_pudr(GPIOC, BUTTON_PIN, PU);
	
	// Output Setting
	GPIO_output(GPIOA, LED_PIN1, Medium, OpenDrain, PU);
	
	// Variable Initialization
 	int stay = 0;
	int push = 0;

	unsigned int LED     = 0;
	unsigned int state_1 = 0;		// LED OFF
	unsigned int state_2 = 1;		// LED ON
	
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
			
			// LED TOGGLE
      if(stay==1 && push==1) {
         if(LED==state_1)
					  LED=state_2;
         else
				    LED=state_1;
				 
      // RUN ONLY ONE TIME AT ONCE
         push=0;
      }
			GPIO_write(GPIOA, LED_PIN1, LED);
	}
*/


/*
	// Part 3. Multiple LEDs On/Off in Sequence
	// Initialization --------------------------------------------------------
	setup();

	// Input Setting
	GPIO_pudr(GPIOC, BUTTON_PIN, PU);
	
	// Output Setting
	GPIO_output(GPIOA, LED_PIN1, Medium, PushPull , PU);
	GPIO_output(GPIOA, LED_PIN2, Medium, PushPull , PU);
	GPIO_output(GPIOA, LED_PIN3, Medium, PushPull , PU);
	GPIO_output(GPIOB, LED_PIN4, Medium, PushPull , PU);
	
	// Variable Initialization
 	int stay = 0;
	int push = 0;

  int i =0;
	int state[5];
	
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
}



// Initialiization 
void setup(void){
	//In Simul , CLK Disable must
	RCC_HSI_init();	
	GPIO_init(GPIOC, BUTTON_PIN, INPUT);   // calls RCC_GPIOC_enable()
	GPIO_init(GPIOA, LED_PIN1, OUTPUT);    // calls RCC_GPIOA_enable()
	GPIO_init(GPIOA, LED_PIN2, OUTPUT);    // calls RCC_GPIOA_enable()
	GPIO_init(GPIOA, LED_PIN3, OUTPUT);    // calls RCC_GPIOA_enable()
	GPIO_init(GPIOB, LED_PIN4, OUTPUT);    // calls RCC_GPIOB_enable()
}
