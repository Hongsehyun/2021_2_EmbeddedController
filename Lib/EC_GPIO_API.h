#ifndef __EC_GPIO_API_H
#define __EC_GPIO_API_H

#include "stm32f4xx.h"
#include "stm32f411xe.h"

#include "ecRCC.h"
#include "ecGPIO.h"
#include "ecEXTI.h"
#include "ecSysTick.h"

#define EC_DOUT  	1
#define EC_DIN 		0

#define EC_PU 1
#define EC_PD 0
#define EC_NONE 0

#define EC_LOW 		0
#define EC_MEDIUM 1
#define EC_FAST 	2
#define EC_HIGH 	3


/* System CLOCK is HSI by default */

class EC_DigitalIn
{
public:
    EC_DigitalIn(GPIO_TypeDef *Port, int pin) 
    {
			uint8_t mode=EC_DIN; // mode=0
			GPIO_init(Port, pin, mode);
			Port_t=Port;
			pin_t=pin;
			mode_t=mode;	
    }

    ~EC_DigitalIn()
    {
			 delete[] Port_t;
    }

    int read();
		
		void pupdr(unsigned int _pupd);
    
    operator int()
    {
        return read();
    }


private:
			GPIO_TypeDef *Port_t;
			int	pin_t;
			int mode_t;	
			int val_t;	
};



class EC_DigitalOut
{
public:
		EC_DigitalOut(GPIO_TypeDef *Port, int pin);
		// Exercise. Define the function in EC_GPIO.cpp

    ~EC_DigitalOut()
    {
			 delete[] Port_t;
    }

    void write(unsigned int _outVal);

  	void pupdr(unsigned int _pupd);
		
		void otype(unsigned int _type);
		
		void ospeed(unsigned int _speed);

		EC_DigitalOut &operator= (unsigned int value)
    {
				 write(value);
				 return *this;
    }
		
		int read();
		
		operator int()
		{ // Underlying call is thread safe
			return read();
		}


private:
			GPIO_TypeDef *Port_t;
			int	pin_t;
			int mode_t;	
};

#endif
