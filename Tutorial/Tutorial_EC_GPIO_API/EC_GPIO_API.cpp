#include "EC_GPIO_API.h"
#include "ecGPIO.h"
#include "ecRCC.h"

#define LED_PIN 	5
#define BUTTON_PIN 13

EC_DigitalIn button(GPIOC,BUTTON_PIN);
EC_DigitalOut led(GPIOA,LED_PIN);

	
int main(void) { 
	while(1){
		if(!button)	led.write(HIGH);			//if(!button.read())
		else 				led.write(LOW);
	}
}