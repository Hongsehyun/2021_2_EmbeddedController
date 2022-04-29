#include "stm32f4xx.h"
#include "ecStepper.h"

//State number 
#define S0 0
#define S1 1
#define S2 2
#define S3 3
#define S4 4
#define S5 5
#define S6 6
#define S7 7

// Stepper Motor function
//static uint32_t direction;
static uint32_t step_delay;

static uint32_t step_per_rev_FULL = 64; // only for FULL-MODE
   // 1 Rotation = 64 Step    // 1 Step     = 5.625 degree

static uint32_t step_per_rev_HALF = 128; // only for HALF-MODE
   // 1 Rotation = 128 Step   // 1 Step     = 2.8125 degree

static uint32_t Gear_Ratio = 32;

// Stepper Motor variable
static volatile Stepper_t myStepper;


//FULL stepping sequence  - FSM
typedef struct {
	uint8_t out;
	uint32_t next[2];
} State_full_t;

//static State_full_t FSM_Test;

static State_full_t FSM_full[4] = {
 {0xC,{S1,S3}},  // A B A' B' : 1100 = C
 {0x6,{S2,S0}},  // A B A' B' : 0110 = 6
 {0X3,{S3,S1}},  // A B A' B' : 0011 = 3
 {0X9,{S0,S2}},  // A B A' B' : 1001 = 9
};


//HALF stepping sequence
typedef struct {
	uint8_t out;
	uint32_t next[2];
} State_half_t;


static State_half_t FSM_half[8] = {
 {0x8,{S1,S7}},  // A B A' B' : 1000 = 8
 {0xC,{S2,S0}},  // A B A' B' : 1100 = C
 {0x4,{S3,S1}},  // A B A' B' : 0100 = 4
 {0X6,{S4,S2}},  // A B A' B' : 0110 = 6
 {0X2,{S5,S3}},  // A B A' B' : 0010 = 2
 {0x3,{S6,S4}},  // A B A' B' : 0011 = 3
 {0x1,{S7,S5}},  // A B A' B' : 0001 = 1
 {0X9,{S0,S6}},  // A B A' B' : 1001 = 9
};


void Stepper_init(GPIO_TypeDef* port1, int pin1, GPIO_TypeDef* port2, int pin2, GPIO_TypeDef* port3, int pin3, GPIO_TypeDef* port4, int pin4) {
	// GPIO Digital Out Initiation
	myStepper.port1 = port1;
	myStepper.pin1  = pin1;
	
	myStepper.port2 = port2;
	myStepper.pin2  = pin2;
	
	myStepper.port3 = port3;
	myStepper.pin3  = pin3;
	
	myStepper.port4 = port4;
	myStepper.pin4  = pin4;
	
	// GPIO Digital Out Initiation
	GPIO_init(myStepper.port1, myStepper.pin1, OUTPUT);
	GPIO_init(myStepper.port2, myStepper.pin2, OUTPUT);
	GPIO_init(myStepper.port3, myStepper.pin3, OUTPUT);
	GPIO_init(myStepper.port4, myStepper.pin4, OUTPUT);
	
  GPIO_output(myStepper.port1, myStepper.pin1, Fast, PushPull, N_PUPD);
  GPIO_output(myStepper.port2, myStepper.pin2, Fast, PushPull, N_PUPD);
  GPIO_output(myStepper.port3, myStepper.pin3, Fast, PushPull, N_PUPD);
  GPIO_output(myStepper.port4, myStepper.pin4, Fast, PushPull, N_PUPD);
}


void Stepper_pinOut(uint32_t state, int mode) {

	if (mode == FULL) {         // FULL mode
		GPIO_write(myStepper.port1, myStepper.pin1, (FSM_full[state].out & 0x8) >> 3);
	  GPIO_write(myStepper.port2, myStepper.pin2, (FSM_full[state].out & 0x4) >> 2);
		GPIO_write(myStepper.port3, myStepper.pin3, (FSM_full[state].out & 0x2) >> 1);
		GPIO_write(myStepper.port4, myStepper.pin4, (FSM_full[state].out & 0x1) >> 0);
	}
	
	else if (mode == HALF) {    // HALF mode
	  GPIO_write(myStepper.port1, myStepper.pin1, (FSM_half[state].out & 0x8) >> 3);
	  GPIO_write(myStepper.port2, myStepper.pin2, (FSM_half[state].out & 0x4) >> 2);
		GPIO_write(myStepper.port3, myStepper.pin3, (FSM_half[state].out & 0x2) >> 1);
		GPIO_write(myStepper.port4, myStepper.pin4, (FSM_half[state].out & 0x1) >> 0);
	}
}


void Stepper_setSpeed(long whatSpeed, int mode){
  if(mode==FULL)   step_delay = (uint32_t)(60000*1000)/(whatSpeed*step_per_rev_FULL*Gear_Ratio);
  if(mode==HALF)	 step_delay = (uint32_t)(60000*1000)/(whatSpeed*step_per_rev_HALF*Gear_Ratio);
  // Convert rpm to milli sec
}


void Stepper_step(int steps, int direction, int mode) {
	
	int state_number = 0;
	int max_state    = 0;
	
	if      (mode==FULL)   max_state = 3;
	else if (mode==HALF)   max_state = 7;

	
	myStepper._step_num = steps;
	for (; myStepper._step_num > 0; myStepper._step_num--) {   // run for step size
		
		if(direction==0) {
			state_number++;	
			if(state_number>max_state) state_number=0;
		}
		
		else if(direction==1) {
			state_number--;
			if(state_number<0) state_number=max_state;
		}
		
		Stepper_pinOut((uint32_t)state_number, mode);
		delay_us(step_delay);                                    // delay (step_delay); 
	}
}


void Stepper_stop(void) {

	myStepper._step_num = 0;
	// All pins(Port1~4, Pin1~4) set as DigitalOut '0'
	GPIO_write(myStepper.port1, myStepper.pin1, myStepper._step_num);
	GPIO_write(myStepper.port2, myStepper.pin2, myStepper._step_num);
  GPIO_write(myStepper.port3, myStepper.pin3, myStepper._step_num);
	GPIO_write(myStepper.port4, myStepper.pin4, myStepper._step_num);
}


/*
void Stepper_run(int direction, int mode){

		Stepper_pinOut(state_number, mode);
	
  	if(direction==0 && mode==HALF) {
			state_number++;
			if(state_number>7) state_number=0;
		}

	 else if(direction==0 && mode==FULL) {
		 state_number++;
	   if(state_number>3) state_number=0;
	 }
	 
	 else if(direction==1 && mode==HALF) {
		 state_number--;
		 if(state_number<0) state_number=7;
	 }
	 
	 else if(direction==1 && mode==FULL) {
		 state_number--;
  	 if(state_number<0) state_number=3;
	 }
}
*/
