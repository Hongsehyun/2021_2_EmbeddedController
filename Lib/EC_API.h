#ifndef __EC_API_H
#define __EC_API_H
#include "stm32f4xx.h"
#include "stm32f411xe.h"
#include "ecRCC.h"
#include "ecGPIO.h"
#include "ecEXTI.h"
#include "ecSysTick.h"

// The three lines below allow the 'C' language to be recognized in 'C++'.
#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */
// The three lines above allow the 'C' language to be recognized in 'C++'.
	 
class EC_Ticker
{
public:
      EC_Ticker(int reload);

      void reset(void);
      void read_ms(void);
      void delay_msec(uint32_t dalayValue);

private:
				 int reload;
         unsigned int dalayValue;
};

// The three lines below allow the 'C' language to be recognized in 'C++'.
#ifdef __cplusplus
}
#endif /* __cplusplus */
// The three lines above allow the 'C' language to be recognized in 'C++'.
#endif
