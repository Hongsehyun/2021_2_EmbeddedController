#include "stm32f4xx.h"
#include "stm32f411xe.h"

#include "ecRCC.h"
#include "ecGPIO.h"
#include "ecEXTI.h"
#include "ecSysTick.h"
#include "EC_API.h"

/* System CLOCK is HSI by default */

// Define EC_Ticker
EC_Ticker::EC_Ticker(int reload)
{
        SysTick_init(reload);
}

void EC_Ticker::reset(void)
{
        SysTick_reset();
}

void EC_Ticker::read_ms(void)
{
        SysTick_val();
}

void EC_Ticker::delay_msec(uint32_t dalayValue)
{
        delay_ms(dalayValue);
}
