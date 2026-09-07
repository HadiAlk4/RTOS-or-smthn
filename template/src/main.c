#include "inc/rtos.h"


// Points to the TCB of the currently active task
volatile TCB_t *CurrentTCB;

//TCB_t Task_List[MAX_NUM_OF_TASKS];  // scheduler list data structure
TCB_t *Task_List = NULL;


// This pointer holds the address of the seven segment display.
// This address points to a block of 8 bytes.
// Each byte corresponds to a display digit.
// Each bit in the byte corresponds to a single segment. 
// See MAX7219 Datasheet for the bit to segment map. 

unsigned char *display_buffer;  // This address is automatically populated by the HAL
volatile uint32_t Ticks=0 ;     // global variable to be used for system timing


/********************************************************************************** */
/* ****  First task run just sets up the system and launches the command shell **** */
/********************************************************************************** */

void initd(int) {
    int id = 1;

// start the Hardware Abstraction Layer (HAL)    
    printf("\nStarting HAL\n");
    if (Hardware_Abstraction_Layer() < 0 ) {
        printf("Error: Failed to Launch HAL - malloc returned NULL\n" );
        while(1) ; // spin lock to trap error
    }

    // load tasks (task, arg0,  name, priority)
    add_Task(&cmdShell,0,"cmdShell",99);

    // keep task in background
    while(1) {}
 }

/* ********************************************************************************* */
/* ******************************* Kernel main ************************************* */
/* ********************************************************************************* */

int main(void)
{
     
    stdio_init_all(); // needed for the Pico stdio functions
    
 
    // wait for the usb serial connections to be established
   while (!stdio_usb_connected()) {}
   while (!stdio_usb_init()) {}

    printf("\nStarting RTOS Kernel\n");

  
    // load tasks (task, arg0,  name, priority) 
    // This will be automatically assigned id = 1 since it is first task loaded into the scheduler
    add_Task(&initd,0,"initd",99);

    // Make current tcb pointer point to the start of the task list
    CurrentTCB = Task_List;
    
    // Set up the systick for the quantum time
    configure_Systick(1) ; // 1 microsecond (1us)

    rtos_Start();   // starting the scheduler (make sure there is something in the Task List first!)

    

    // Error Trap for failed RTOS
    printf("\nError: Should never get here\n");
    while (true) {
        __asm("nop");
    }

    return 0;
}


