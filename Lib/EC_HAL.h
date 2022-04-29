#ifndef __EC_HAL_H
#define __EC_HAL_H


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
	// The three lines above allow the 'C' language to be recognized in 'C++'.

	#include "stm32f4xx.h"
	#include "stm32f411xe.h"
	
	#include "ecGPIO.h"
	#include "ecRCC.h"
	#include "ecSysTick.h"
	#include "ecEXTI.h"
	#include "ecTIM.h"
	#include "ecPWM.h"
	#include "ecStepper.h"
	#include "ecUART.h"
	#include "ecADC.h"

	// The three lines below allow the 'C' language to be recognized in 'C++'.
#ifdef __cplusplus
}
#endif /* __cplusplus */
// The three lines above allow the 'C' language to be recognized in 'C++'.
#endif
