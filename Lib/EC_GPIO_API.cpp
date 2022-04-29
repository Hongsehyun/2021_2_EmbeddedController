#include "EC_GPIO_API.h"
#include "stm32f4xx.h"
#include "stm32f411xe.h"

#include "ecRCC.h"
#include "ecGPIO.h"
#include "ecEXTI.h"
#include "ecSysTick.h"

/* System CLOCK is HSI by default */


// Define EC_DigitalIn
int EC_DigitalIn::read()
{
	int val_t = GPIO_read(Port_t, pin_t);
	return val_t;
}

void EC_DigitalIn::pupdr(unsigned int _pupd)
{
	GPIO_pudr(Port_t, pin_t, _pupd);
}



// Define EC_DigitalOut
EC_DigitalOut::EC_DigitalOut(GPIO_TypeDef *Port, int pin) 
{
			uint8_t mode=EC_DOUT;  // mode=1;			
			GPIO_init(Port, pin, mode);
			this->Port_t=Port;
			this->pin_t=pin;
			this->mode_t=mode;	
}
	
void EC_DigitalOut::write(unsigned int _outVal)
{
		  GPIO_write(Port_t, pin_t, _outVal);
}

void EC_DigitalOut::pupdr(unsigned int _pupd){
			// assert(_pupd)
			// Error Checking statement
			// int_validcheck = assert(_pupd);
		  GPIO_pudr(Port_t, pin_t, _pupd);
}

void EC_DigitalOut::otype(unsigned int _type){
			GPIO_otype(Port_t, pin_t, _type);
}

void EC_DigitalOut::ospeed(unsigned int _speed){
			GPIO_ospeed(Port_t, pin_t, _speed);
}

int EC_DigitalOut::read(){
			return GPIO_read(Port_t, pin_t);
}
