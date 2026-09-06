#include "inc/rtos.h"

// Remember that these tasks can not be allowed to complete!
//          Use an infinite loop so that the function never exits. 


/* ===========================================================================
 * Blinky - flashes the onboard LED 'count' times (ON 2s / OFF 2s per
 * flash), then leaves it off.
 * ===========================================================================
 */
void blink(int numflash)
{

}

/* ===========================================================================
 * Counter - continuously counts 00..99, updating every 0.5 s. Tens digit
 * at 'pos', units digit at 'pos + 1'.
 * ===========================================================================
 */
void count(int pos)
{
   
}

/* ===========================================================================
 * Flasher - flashes the decimal point at 'pos' at 2 Hz (0.25 s on,
 * 0.25 s off).
 * ===========================================================================
 */
void flash( int pos)
{
   
}

/* ===========================================================================
 * Hexer - displays a ping-pong hex count at 'pos': F,E,...,0,1,...,F,
 * repeating indefinitely, changing once per second.
 * ===========================================================================
 */
void hexer( int pos )
{
   
}

/* ===========================================================================
 * Splat - "chases" the segments of digit 'pos' on one at a time
 * (A, A+B, A+B+C, ... , all 8 including DP), then removes them one at a
 * time in reverse (A..G, A..F, ... , OFF), repeating continuously. Each
 * step is held for 0.25 s.
 * ===========================================================================
 */
void splat(int pos)
{
    
}

