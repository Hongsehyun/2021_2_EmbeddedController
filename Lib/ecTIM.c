#include "stm32f4xx.h"
#include "stm32f411xe.h"

#include "ecTIM.h"
#include "ecGPIO.h"

// PSC = Prescaler
// ARR = Auto Reload Register
// CCR = Compare Capture Register

/* Timer Configuration */
void TIM_init(TIM_TypeDef* timerx, uint32_t msec){ 
	
  // 1. Enable Timer Peripheral CLOCK
	if(timerx ==TIM1)       RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	else if(timerx ==TIM2)  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	else if(timerx ==TIM3)  RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	else if(timerx ==TIM4)  RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
	else if(timerx ==TIM5)  RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
	else if(timerx ==TIM9)  RCC->APB2ENR |= RCC_APB2ENR_TIM9EN;
	else if(timerx ==TIM11) RCC->APB2ENR |= RCC_APB2ENR_TIM11EN;
	
	//   2. Set CNT period  // Define PSD and ARR Value
	//// At Tutorial Part 1,2 --- used this code for here ::
	//   TIM_period_us(timerx,msec);  
  //// At Lab --- used this code for here ::
	TIM_period_ms(timerx,msec);
	
	// 3. CNT Direction
	timerx->CR1 &= ~TIM_CR1_DIR;			   		// Upcounter	
	// timerx->CR1 |= TIM_CR1_DIR;					// Down counter	
  
  timerx->DIER |= TIM_DIER_UIE; 
  //timerx->DIER |= TIM_DIER_CC; 

  // 4. Enable Timer Counter
	timerx->CR1 |= TIM_CR1_CEN;		
}

// Update Event Interrupt
void TIM_INT_init(TIM_TypeDef* timerx, uint32_t msec){
  // 1. Initialize Timer	
	TIM_init(timerx,msec);
	
  // 2. Enable Update Interrupt
	uint32_t IRQn_reg =0;
	
	if(timerx ==TIM1)        IRQn_reg = TIM1_UP_TIM10_IRQn;
	else if(timerx ==TIM2)   IRQn_reg = TIM2_IRQn;
	else if(timerx ==TIM3)   IRQn_reg = TIM3_IRQn;
	else if(timerx ==TIM4)   IRQn_reg = TIM4_IRQn;
	else if(timerx ==TIM5)   IRQn_reg = TIM5_IRQn;
	else if(timerx ==TIM9)   IRQn_reg = TIM1_BRK_TIM9_IRQn;
	else if(timerx ==TIM11)  IRQn_reg = TIM1_TRG_COM_TIM11_IRQn;
  
	NVIC_EnableIRQ(IRQn_reg);				
	NVIC_SetPriority(IRQn_reg,5);
}

void TIM_INT_enable(TIM_TypeDef* timerx){
	timerx->DIER |= TIM_DIER_UIE;		//Enable Timer Update Interrupt		
}

void TIM_INT_disable(TIM_TypeDef* timerx){
	timerx->DIER &= ~TIM_DIER_UIE;	//Disable Timer Update Interrupt		
}

// update interrupt flag       UIE
uint32_t is_UIF(TIM_TypeDef *TIMx){
	return ((TIMx->SR & TIM_SR_UIF) == TIM_SR_UIF); 
}

void clear_UIF(TIM_TypeDef *TIMx){
	TIMx->SR &= ~TIM_SR_UIF;
}

void TIM_period_ms(TIM_TypeDef* TIMx, uint32_t msec){ 
	// Period msec = 1 to 6000
	// 0.1ms(10kHz, ARR=1) to 6.5sec (ARR=0xFFFF)
	uint32_t prescaler;
	uint32_t ARRval;
	
	// Check System CLK: PLL or HSI
	if((RCC->CFGR & (3<<0)) == 2)      { prescaler = 8400; }  // for msec 84MHz/1000
	else if((RCC->CFGR & (3<<0)) == 0) { prescaler = 1600; }
	
  //	uint32_t prescaler = 8400;
	ARRval = 10 * msec ;
	
	TIMx->PSC = prescaler-1;					
	TIMx->ARR = ARRval-1;							
}

//	Q. Which combination of PSC and ARR for msec unit?
// 	Q. What are the possible range (in sec ?)
void TIM_period_us(TIM_TypeDef *TIMx, uint32_t usec){   
	// Period usec = 1 to 1000
	// 1us(1MHz, ARR=1) to 65msec (ARR=0xFFFF)
	uint32_t prescaler;
	uint32_t ARRval;
	
	// Check System CLK: PLL or HSI
	if((RCC->CFGR & (3<<0)) == 2)      { prescaler = 84; }
	else if((RCC->CFGR & (3<<0)) == 0) { prescaler = 16; }
	
	ARRval = usec;

	TIMx->PSC = prescaler-1;					
	TIMx->ARR = ARRval-1;					
}

void TIM_period_us_SmartHome(TIM_TypeDef *TIMx, uint32_t usec){   
	uint32_t prescaler = 84;
	if(TIMx==TIM2) {
	  uint32_t ARRval_32= (84/(prescaler)*usec); // 84MHz/1000000 us
	  TIMx->ARR = ARRval_32-1;
	}
	else{
	  uint16_t ARRval= (84/(prescaler)*usec); // 84MHz/1000000 us
	  TIMx->ARR = ARRval-1;
	}
	TIMx->PSC = prescaler-1;
}

/* Input Capture Configuration _ ecIC.c */
void ICAP_init(IC_t *ICx, GPIO_TypeDef *port, int pin){
  
	// 0. Match Input Capture Port and Pin for TIMx
	ICx->port = port;
	ICx->pin  = pin;
	ICAP_pinmap(ICx);	  										// Port, Pin --(mapping)--> TIMx, Channel
	
	TIM_TypeDef *TIMx = ICx->timer;
	int TIn = ICx->ch; 		
	int ICn=TIn;
	ICx->ICnum=ICn;													// (default) TIx=ICx

  /************************* GPIO configuration *************************/
	/************************* GPIO configuration *************************/
  // 1. Initialize GPIO port and pin as AF
	GPIO_init(port, pin, AF);                 // AF=2
	GPIO_pudr(port, pin, N_PUPD); 

  // 2. Configure GPIO AFR by Pin num.
  port->AFR[pin>>3] &= ~(0xF<<(4*(pin%8)));
	
	if(TIMx==TIM1 || TIMx==TIM2)                         port->AFR[pin>>3] |= 1U << 4*(pin%8); 			
	else if(TIMx==TIM3 || TIMx==TIM4  || TIMx==TIM5)     port->AFR[pin>>3] |= 2U << 4*(pin%8);
  else if(TIMx==TIM9 || TIMx==TIM10 || TIMx==TIM11)    port->AFR[pin>>3] |= 3U << 4*(pin%8);
	
	/************************* TIMER configuration *************************/
	/************************* TIMER configuration *************************/
	// 1. Initialize Timer 
  TIM_init(TIMx, 100);
  
	// 2. Initialize Timer Interrpt 
	TIM_INT_init(TIMx, 100);        					// TIMx Interrupt initialize 
  
	// 3. Modify ARR Maximum for 1MHz
	TIMx->PSC = 84-1;						  					// Timer counter clock: 1MHz(1us)  for PLL
	TIMx->ARR = 0xFFFF;											// Set auto reload register to maximum (count up to 65535)
  
	// 4. Disable Counter during configuration
	TIMx->CR1 &= ~TIM_CR1_CEN;  						// Disable Counter during configuration

  /************************* Input Capture Configuration *************************/
  /************************* Input Capture Configuration *************************/
  // 1. Select Timer channel(TIx) for Input Capture channel(ICx)
	// Default Setting
	TIMx->CCMR1 |= 	TIM_CCMR1_CC1S_0;     //01<<0   CC1S    TI1=IC1
	TIMx->CCMR1 |= 	TIM_CCMR1_CC2S_0;     //01<<8   CC2s    TI2=IC2
	TIMx->CCMR2 |= 	TIM_CCMR2_CC3S_0;     //01<<0   CC3s    TI3=IC3
	TIMx->CCMR2 |= 	TIM_CCMR2_CC4S_0;  		//01<<8   CC4s    TI4=IC4

  // 2. Filter Duration (use default)
  
  // 3. IC Prescaler (use default)

  // 4. Activation Edge: CCyNP/CCyP	
	TIMx->CCER &= ~(15UL<<((ICn-1)*4));     // clear
	TIMx->CCER &= ~(5UL<<(((ICn-1)*4)+1));  // Capture Operation : non-inverting and rising edge
		 
  // 5.	Enable CCy Capture, Capture/Compare interrupt
	TIMx->CCER |= 1UL << ((ICn-1)*4);			  // CCn(ICn) Capture Enable	

  // 6.	Enable Interrupt of CC(CCyIE), Update (UIE)
	TIMx->DIER |= 1UL<<ICn;					// Capture/Compare Interrupt Enable	for ICn
	TIMx->DIER |= TIM_DIER_UIE;							// Update Interrupt enable	

  // 7.	Enable Counter 
	TIMx->CR1	 |= TIM_CR1_CEN;							// Counter enable	
}

// Configure Selecting TIx-ICy and Edge Type
void ICAP_setup(IC_t *ICx, int ICn, int edge_type){
	TIM_TypeDef *TIMx = ICx->timer;	// TIMx
	int 				  CHn = ICx->ch;		// Timer Channel CHn
	ICx->ICnum=ICn;

  // Disable  CC. Disable CCInterrupt for ICn. 
	TIMx->CCER &= ~(1<<(4*(ICn-1)));   		// Capture Disable
	TIMx->DIER &= ~(1UL<<ICn);						// CCn Interrupt Disable	
	 
  // Configure  IC number(user selected) with given IC pin(TIMx_CHn)
	switch(ICn){
			case 1:
					TIMx->CCMR1 &= ~TIM_CCMR1_CC1S;											//reset   CC1S
					if   (ICn==CHn) TIMx->CCMR1 |= TIM_CCMR1_CC1S_0;    //01<<0   CC1S    Tx_Ch1=IC1
					else TIMx->CCMR1 |= TIM_CCMR1_CC1S_1;               //10<<0   CC1S    Tx_Ch2=IC1
					break;
			case 2:
					TIMx->CCMR1 &= ~TIM_CCMR1_CC2S;								  		//reset   CC2S
					if   (ICn==CHn) TIMx->CCMR1 |= 	TIM_CCMR1_CC2S_0;   //01<<0   CC2S    Tx_Ch2=IC2
					else TIMx->CCMR1 |= TIM_CCMR1_CC2S_1;       				//10<<0   CC2S    Tx_Ch1=IC2
					break;
			case 3:
					TIMx->CCMR2 &= ~TIM_CCMR2_CC3S;											//reset   CC3S
					if   (ICn==CHn) TIMx->CCMR2 |= TIM_CCMR2_CC3S_0;	  //01<<0   CC3S    Tx_Ch3=IC3
					else TIMx->CCMR2 |= TIM_CCMR2_CC3S_1; 	     				//10<<0   CC3S    Tx_Ch4=IC3
					break;
			case 4:
					TIMx->CCMR2 &= ~TIM_CCMR2_CC4S;									  	//reset   CC4S
					if   (ICn==CHn) TIMx->CCMR2 |= TIM_CCMR2_CC4S_0;	  //01<<0   CC4S    Tx_Ch4=IC4
					else TIMx->CCMR2 |= TIM_CCMR2_CC4S_1;     					//10<<0   CC4S    Tx_Ch3=IC4
					break;
			default: break;
		}

	// Configure Activation Edge direction
	TIMx->CCER &= ~(5<<(((ICn-1)*4)+1));	  									// Clear CCnNP/CCnP bits for ICn
	switch(edge_type){
		case RISE: TIMx->CCER &= ~(5<<(((ICn-1)*4)+1));	 break; //rising:  00
		case FALL: TIMx->CCER |= (1<<(((ICn-1)*4)+1));	 break; //falling: 01
		case BOTH: TIMx->CCER |= (5<<(((ICn-1)*4)+1));	 break; //both:    11
	}
	
  // Enable CC. Enable CC Interrupt. 
	TIMx->CCER |= 1 << (4*(ICn - 1)); 										// Capture Enable
	TIMx->DIER |= 1 << ICn; 															// CCn Interrupt enabled	
}

// Time span for one counter step
void ICAP_counter_us(IC_t *ICx, int usec){	
	TIM_TypeDef *TIMx = ICx->timer;	
	TIMx->PSC = 84*usec-1;						  // Timer counter clock: 1us * usec
	TIMx->ARR = 0xFFFF;									// Set auto reload register to maximum (count up to 65535)
}

uint32_t is_CCIF(TIM_TypeDef *TIMx, uint32_t ccNum){
	switch(ccNum){
    case 1 : return ((TIMx->SR & TIM_SR_CC1IF) == TIM_SR_CC1IF); 	break;
    case 2 : return ((TIMx->SR & TIM_SR_CC2IF) == TIM_SR_CC2IF); 	break;
    case 3 : return ((TIMx->SR & TIM_SR_CC3IF) == TIM_SR_CC3IF); 	break;
    case 4 : return ((TIMx->SR & TIM_SR_CC4IF) == TIM_SR_CC4IF); 	break;
  }
}

void clear_CCIF(TIM_TypeDef *TIMx, uint32_t ccNum){
	switch(ccNum){
    case 1 : TIMx->SR &= ~TIM_SR_CC1IF; break;
    case 2 : TIMx->SR &= ~TIM_SR_CC2IF;	break;
    case 3 : TIMx->SR &= ~TIM_SR_CC3IF; break;
    case 4 : TIMx->SR &= ~TIM_SR_CC4IF;	break;
  }
}

//DO NOT MODIFY THIS
void ICAP_pinmap(IC_t *timer_pin){
   GPIO_TypeDef *port = timer_pin->port;
   int pin = timer_pin->pin;
   
   if(port == GPIOA) {
      switch(pin){
         case 0 : timer_pin->timer = TIM2; timer_pin->ch = 1; break;
         case 1 : timer_pin->timer = TIM2; timer_pin->ch = 2; break;
         case 5 : timer_pin->timer = TIM2; timer_pin->ch = 1; break;
         case 6 : timer_pin->timer = TIM3; timer_pin->ch = 1; break;
         //case 7: timer_pin->timer = TIM1; timer_pin->ch = 1N; break;
         case 8 : timer_pin->timer = TIM1; timer_pin->ch = 1; break;
         case 9 : timer_pin->timer = TIM1; timer_pin->ch = 2; break;
         case 10: timer_pin->timer = TIM1; timer_pin->ch = 3; break;
         case 15: timer_pin->timer = TIM2; timer_pin->ch = 1; break;
         default: break;
      }         
   }
   else if(port == GPIOB) {
      switch(pin){
         //case 0: timer_pin->timer = TIM1; timer_pin->ch = 2N; break;
         //case 1: timer_pin->timer = TIM1; timer_pin->ch = 3N; break;
         case 3 : timer_pin->timer = TIM2; timer_pin->ch = 2; break;
         case 4 : timer_pin->timer = TIM3; timer_pin->ch = 1; break;
         case 5 : timer_pin->timer = TIM3; timer_pin->ch = 2; break;
         case 6 : timer_pin->timer = TIM4; timer_pin->ch = 1; break;
         case 7 : timer_pin->timer = TIM4; timer_pin->ch = 2; break;
         case 8 : timer_pin->timer = TIM4; timer_pin->ch = 3; break;
         case 9 : timer_pin->timer = TIM4; timer_pin->ch = 3; break;
         case 10: timer_pin->timer = TIM2; timer_pin->ch = 3; break;
         default: break;
      }
   }
   else if(port == GPIOC) {
      switch(pin){
         case 6 : timer_pin->timer = TIM3; timer_pin->ch = 1; break;
         case 7 : timer_pin->timer = TIM3; timer_pin->ch = 2; break;
         case 8 : timer_pin->timer = TIM3; timer_pin->ch = 3; break;
         case 9 : timer_pin->timer = TIM3; timer_pin->ch = 4; break;
         default: break;
      }
   }
}



/*
// Compare Capture Interrupt Flag
// CC1IF
uint32_t is_CC1IF(TIM_TypeDef *TIMx){
	return ((TIMx->SR & TIM_SR_CC1IF) == TIM_SR_CC1IF); 
}

void clear_CC1IF(TIM_TypeDef *TIMx){
	TIMx->SR &= ~TIM_SR_CC1IF;
}

// CC2IF
uint32_t is_CC2IF(TIM_TypeDef *TIMx){
	return ((TIMx->SR & TIM_SR_CC2IF) == TIM_SR_CC2IF); 
}

void clear_CC2IF(TIM_TypeDef *TIMx){
	TIMx->SR &= ~TIM_SR_CC2IF;
}

// CC3IF
uint32_t is_CC3IF(TIM_TypeDef *TIMx){
	return ((TIMx->SR & TIM_SR_CC3IF) == TIM_SR_CC3IF); 
}

void clear_CC3IF(TIM_TypeDef *TIMx){
	TIMx->SR &= ~TIM_SR_CC3IF;
}

// CC4IF
uint32_t is_CC4IF(TIM_TypeDef *TIMx){
	return ((TIMx->SR & TIM_SR_CC4IF) == TIM_SR_CC4IF); 
}

void clear_CC4IF(TIM_TypeDef *TIMx){
	TIMx->SR &= ~TIM_SR_CC4IF;
}
*/
