#include "inc/rtos.h"

// returns 0 if the count > 24Bits
uint32_t configure_Systick(uint32_t micro_seconds)
{
    
}


// just use the systick to trigger the context switch
// This is also where the Ticks are generated and used to call the scheduler
// The RTOS System Ticks are also managed in this systick handler

// Note that this function is commented out so that it uses the template supplied one.
/*
void isr_systick(void) {
    CONTEXT_SWITCH();    // trigger context switch and calls the scheduler
    }
*/
