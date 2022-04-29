#ifndef __ECGPIO_H
#define __ECGPIO_H

// Distributed for LAB: GPIO
#include "stm32f4xx.h"
#include "stm32f411xe.h"

#include "ecRCC.h"  // CALLing CLK

// Pin Mode
#define INPUT  0x00
#define OUTPUT 0x01
#define AF     0x02
#define ANALOG 0x03

//GPIO Push-Pull    
#define N_PUPD      0x00
#define PU          0x01
#define PD          0x02
#define Reserved    0x03

//GPIO Speed
#define Low         0x00
#define Medium      0x01
#define Fast        0x02
#define High        0x03

//GPIO Output Type
#define PushPull    0x00
#define OpenDrain   0x01

#define LOW  0
#define HIGH 1


#define LED_PIN 	 5
#define LED_PIN1 	 5
#define LED_PIN2 	 6
#define LED_PIN3 	 7
#define LED_PIN4 	 6
#define LED_PIN5 	 7
#define LED_PIN6 	 9
#define LED_PIN7 	 8
#define LED_PIN8 	 10
#define BUTTON_PIN 13


// The three lines below allow the 'C' language to be recognized in 'C++'.
#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */
// The three lines above allow the 'C' language to be recognized in 'C++'.


// LAB2. Create EC_HAL Driver
void GPIO_init(GPIO_TypeDef *Port, int pin, unsigned int mode);
void RCC_GPIO_enable(GPIO_TypeDef *Port); 

void      GPIO_write(GPIO_TypeDef *Port, int pin, unsigned int Output);
uint32_t	GPIO_read(GPIO_TypeDef *Port, int pin);

void GPIO_mode(GPIO_TypeDef* Port, int pin, unsigned int mode);
void GPIO_ospeed(GPIO_TypeDef* Port, int pin, unsigned int speed);
void GPIO_otype(GPIO_TypeDef* Port, int pin, unsigned int type);
void GPIO_pudr(GPIO_TypeDef* Port, int pin, unsigned int pudr);

// Output Setting Function
void GPIO_output(GPIO_TypeDef* Port, int pin, unsigned int speed, unsigned int type, unsigned int pudr);

	 
// LAB3. 7 Segment
void sevensegment_init(int pin1, int pin2, int pin3, int pin4, int pin5, int pin6, int pin7, int pin8);
void sevensegment_decode(unsigned int cnt);


// LAB4.
void LED_toggle(GPIO_TypeDef *Port, unsigned int pin);


// The three lines below allow the 'C' language to be recognized in 'C++'.
#ifdef __cplusplus
}
#endif /* __cplusplus */
// The three lines above allow the 'C' language to be recognized in 'C++'.

#endif
