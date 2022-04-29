// Distributed for LAB: GPIO
#include "stm32f4xx.h"
#include "stm32f411xe.h"

#include "ecGPIO.h"


void GPIO_init(GPIO_TypeDef *Port, int pin, unsigned int mode){     
   
	if (Port == GPIOA)
		RCC_GPIOA_enable();
	if (Port == GPIOB)
		RCC_GPIOB_enable();
	if (Port == GPIOC)
		RCC_GPIOC_enable();

	GPIO_mode(Port, pin, mode);
	// The rest are default values
}


void GPIO_write(GPIO_TypeDef *Port, int pin, unsigned int Output) {
	Port->ODR &= ~(1<<(pin))  ;
	Port->ODR |= (Output<<(pin));
}


uint32_t  GPIO_read(GPIO_TypeDef *Port, int pin) {
  return (Port->IDR) & (1UL<<(pin));
}


// GPIO Mode          : Input(00), Output(01), AlterFunc(10), Analog(11, reset)
void GPIO_mode(GPIO_TypeDef *Port, int pin, unsigned int mode){
   Port->MODER &= ~(3UL<<(2*pin));     
   Port->MODER |= mode <<(2*pin) ;    
}


void GPIO_ospeed(GPIO_TypeDef* Port, int pin, unsigned int speed) {
	Port->OSPEEDR &= ~(3UL<<(2*pin));     
  Port->OSPEEDR |= speed<<(2*pin) ;
}


void GPIO_otype(GPIO_TypeDef* Port, int pin, unsigned int type) {
	Port->OTYPER &= ~(1UL<<(pin));     
	Port->OTYPER |= type <<(pin) ;     
}


void GPIO_pudr(GPIO_TypeDef* Port, int pin, unsigned int pudr) {
  Port->PUPDR &= ~(3UL<<(2*pin));     
	Port->PUPDR |= pudr <<(2*pin) ;     
}


// Output Setting Function
void GPIO_output(GPIO_TypeDef* Port, int pin, unsigned int speed, unsigned int type, unsigned int pudr) {
	Port->OSPEEDR &= ~(3UL<<(2*pin));     
  Port->OSPEEDR |= speed<<(2*pin) ;
	Port->OTYPER &= ~(1UL<<(pin));     
	Port->OTYPER |= type <<(pin) ;    
	Port->PUPDR &= ~(3UL<<(2*pin));     
	Port->PUPDR |= pudr <<(2*pin) ; 
}


// LAB3. 7 Segment
void sevensegment_init(int pin1, int pin2, int pin3, int pin4, int pin5, int pin6, int pin7, int pin8) {
	GPIO_init(GPIOA, pin1, OUTPUT);    // calls RCC_GPIOA_enable()
	GPIO_init(GPIOA, pin2, OUTPUT);    // calls RCC_GPIOA_enable()
	GPIO_init(GPIOA, pin3, OUTPUT);    // calls RCC_GPIOA_enable()
	GPIO_init(GPIOB, pin4, OUTPUT);    // calls RCC_GPIOB_enable()
	GPIO_init(GPIOC, pin5, OUTPUT);    // calls RCC_GPIOC_enable()
	GPIO_init(GPIOA, pin6, OUTPUT);    // calls RCC_GPIOA_enable()
	GPIO_init(GPIOA, pin7, OUTPUT);    // calls RCC_GPIOA_enable()
	GPIO_init(GPIOB, pin8, OUTPUT);    // calls RCC_GPIOA_enable()
}


void sevensegment_decode(unsigned int cnt) {
			
			unsigned int SEGnum[11][8]={
                    {0,0,0,0,0,0,1,1},          //zero
                    {1,0,0,1,1,1,1,1},          //one
                    {0,0,1,0,0,1,0,1},          //two
                    {0,0,0,0,1,1,0,1},          //three
                    {1,0,0,1,1,0,0,1},          //four
                    {0,1,0,0,1,0,0,1},          //five
                    {0,1,0,0,0,0,0,1},          //six
                    {0,0,0,1,1,0,1,1},          //seven
                    {0,0,0,0,0,0,0,1},          //eight
                    {0,0,0,0,1,0,0,1},          //nine
                    {1,1,1,1,1,1,1,0}           //dot
                    };
			
				GPIO_write(GPIOA, LED_PIN1, SEGnum[cnt][0]);
				GPIO_write(GPIOA, LED_PIN2, SEGnum[cnt][1]);
				GPIO_write(GPIOA, LED_PIN3, SEGnum[cnt][2]);
				GPIO_write(GPIOB, LED_PIN4, SEGnum[cnt][3]);
				GPIO_write(GPIOC, LED_PIN5, SEGnum[cnt][4]);
				GPIO_write(GPIOA, LED_PIN6, SEGnum[cnt][5]);
				GPIO_write(GPIOA, LED_PIN7, SEGnum[cnt][6]);
				GPIO_write(GPIOB, LED_PIN8, SEGnum[cnt][7]);									
}

// LAB4.
void LED_toggle(GPIO_TypeDef *Port, unsigned int pin) {
		Port->ODR ^= (1<<(pin))  ;  
}
