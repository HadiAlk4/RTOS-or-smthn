#include "inc/rtos.h"
#include "pico/stdlib.h"
// Remember that these tasks can not be allowed to complete!
//          Use an infinite loop so that the function never exits. 


/* ===========================================================================
 * Blinky - flashes the onboard LED 'count' times (ON 2s / OFF 2s per
 * flash), then leaves it off.
 * ===========================================================================
 */


// add_Task() creates a TCB and a private stack, 
// and the scheduler later sets the CPU’s PC to the start of blink. 
// There is no C caller waiting for a return value.
void blink(int numflash)
{

const uint LED_PIN = 25;
gpio_init(LED_PIN);
gpio_set_dir(LED_PIN, GPIO_OUT);

for(int i=0;i<numflash;i++)
{
gpio_put(LED_PIN, 1);
sleep_ms(2000);
gpio_put(LED_PIN, 0);
sleep_ms(2000);
}
gpio_put(LED_PIN, 0);
while (1) {}

// while (true) 
// {
// gpio_put(LED_PIN, 1);
// sleep_ms(5000);
// gpio_put(LED_PIN, 0);
// sleep_ms(5000);
// }
}

/* ===========================================================================
 * Counter - continuously counts 00..99, updating every 0.5 s. Tens digit
 * at 'pos', units digit at 'pos + 1'.
 * ===========================================================================
 */

/* DP A B C D E F G  — MAX7219 no-decode */
char hex_font[] = 
{
    0x7E, /* 0: A B C D E F     */
    0x30, /* 1: B C             */
    0x6D, /* 2: A B D E G       */
    0x79, /* 3: A B C D G       */
    0x33, /* 4: B C F G         */
    0x5B, /* 5: A C D F G       */
    0x5F, /* 6: A C D E F G     */
    0x70, /* 7: A B C           */
    0x7F, /* 8: A B C D E F G   */
    0x7B, /* 9: A B C D F G     */
    0x77, /* A: A B C E F G     */
    0x1F, /* b: C D E F G       */
    0x4E, /* C: A D E F         */
    0x3D, /* d: B C D E G       */
    0x4F, /* E: A D E F G       */
    0x47, /* F: A E F G         */

};

void count(int pos)
{
    int n=0;
     while(1)
     {
        display_buffer[pos] = hex_font[n/10];
        display_buffer[pos+1] = hex_font[n%10];
        delay(500);
        n++;
        if(n>99)
        {
            n=0;
        }
     }

}

/* ===========================================================================
 * Flasher - flashes the decimal point at 'pos' at 2 Hz (0.25 s on,
 * 0.25 s off).
 * ===========================================================================
 */
void flash( int pos)
{
    while(1)
    {
        display_buffer[pos] = 0x80; // on
        delay(250);

        display_buffer[pos] = 0x00; // off 
        delay(250);
    }
}

/* ===========================================================================
 * Hexer - displays a ping-pong hex count at 'pos': F,E,...,0,1,...,F,
 * repeating indefinitely, changing once per second.
 * ===========================================================================
 */
void hexer( int pos )
{
    int n=15;
    int dir=-1;

    while(1)
    {
        display_buffer[pos] = hex_font[n];
        delay(1000);

        n += dir;
        if (n == 0 || n == 15)
        {
            dir = -dir;
        }
    }
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
    char segment[] = 
    {
        0x40, /* A  */
        0x20, /* B  */
        0x10, /* C  */
        0x08, /* D  */
        0x04, /* E  */
        0x02, /* F  */
        0x01, /* G  */
        0x80  /* DP */
    };

    while(1)
    {
        char pattern = 0;

        for(int i=0; i<8;i++)
        {
            pattern |= segment[i];
            display_buffer[pos] = pattern;
            delay(250);
        }

        for(int i=7; i>=0;i--)
        {
            pattern &= ~segment[i];
            display_buffer[pos] = pattern;
            delay(250);
        }
    }
    
}

