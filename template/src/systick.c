#include "inc/rtos.h"

// returns 0 if the count > 24Bits
uint32_t configure_Systick(uint32_t micro_seconds)
{
    if (micro_seconds > 0x00FFFFFEu) return 0;
    systick_hw->csr |= 1u | 2u | 4u;
    systick_hw->rvr = micro_seconds * 125u - 1u;
    return systick_hw->cvr;
}


// just use the systick to trigger the context switch
// This is also where the Ticks are generated and used to call the scheduler
// The RTOS System Ticks are also managed in this systick handler

// Note that this function is commented out so that it uses the template supplied one.

void isr_systick(void) 
{
    Ticks++;                              // 1 µs system time
    if ((Ticks % TICKS_BEFORE_SWAP) == 0) // every 100 µs = one quantum
    CONTEXT_SWITCH();    // trigger context switch and calls the scheduler
}

