#ifndef RTOS_H
#define RTOS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "pico/stdlib.h"                // GPIO, timing, stdio, core Pico runtime support
#include "pico/malloc.h"                // Heap allocation functions (malloc/free) for Pico SDK
#include "pico/platform.h"              // Platform/compiler attributes, SDK portability macros

#include "hardware/structs/systick.h"  // Direct register access to SysTick timer
#include "hardware/structs/nvic.h"     // NVIC interrupt controller register definitions
#include "hardware/structs/scb.h"      // System Control Block: priorities, exceptions, ICSR
#include "hardware/irq.h"              // IRQ API: enable, disable, set handlers, exceptions



// Task & stack sizes
#define MAX_NUM_OF_TASKS   10
#define STACK_SIZE         1024

/*****************************************************************************************/
/**************************** Task Control Block Structure *******************************/
/*****************************************************************************************/
// Task control block, implemented as a linked list to point to the TCB of the next task.

struct tcb {
    uint32_t        *Stack_Pointer;   // MUST always be the first member for context switch to work
    uint32_t        id;               // unique task id
    uint8_t         priority;         // priority assigned to task (integer - you decide how teh number relates to the priority)
    char            name[32];         // name given to the task. Used in the cmdShell to launch the task 
    uint32_t        TCB_Stack[STACK_SIZE] ;   // the stack for the task / thread
    struct tcb      *next;           // Pointer to next task in the list (NULL if no next task)          
};

typedef struct tcb TCB_t; // Define tcb_t datatype


/*****************************************************************************************/
/**************************** Task List **************************************************/
/*****************************************************************************************/

extern volatile TCB_t *CurrentTCB;  // Points to the TCB of the currently active task
extern TCB_t *Task_List;   // list of task control blocks


/***********************************************************/
/******************* Task / Timing Control ****************/
/**********************************************************/

extern volatile uint32_t Ticks ; // global Tick for System Timing


#define ICSR_PENDSVSET        (1u << 28)
#define CONTEXT_SWITCH()      (scb_hw->icsr = ICSR_PENDSVSET)

// Interrupt Macros
#define DISABLE_INT()       __asm volatile ("cpsid i")
#define ENABLE_INT()        __asm volatile ("cpsie i")


#define TICKS_BEFORE_SWAP 100           // How many clock ticks required before the scheduler is called


/*****************************************************************/
/*************** Helper Function Prototypes *********************/
/***************************************************************/

// Required functions
extern void rtos_Start(void) ;
extern void readln(char *line, int size) ;
extern void delay(uint32_t ms) ;


// OS Operation functions
extern uint32_t configure_Systick(uint32_t timer_value) ;
extern void scheduler(void)  ;
extern int add_Task(void (*Task)(int), uint32_t arg, const char *name, uint8_t priority);
extern int remove_Task(uint32_t id) ;


/*****************************************************************/
/************************* HAL **********************************/
/****************************************************************/

// HAL to drive the SPI for the 7seg board
// display_buffer points to the shared memory between cores that represents the display. ( 8 bytes for 8 digits ). 
// Each byte corresponds to a digit
// Each bit in the byte corresponds to the segment. See MAX7219 Datasheet

int Hardware_Abstraction_Layer(void);   // -ve value means it could not startup
extern unsigned char *display_buffer;   // address of memory where each byte rpresents a digit



/*****************************************************************/
/******************** Task Prototypes ***************************/
/****************************************************************/

extern void blink(int) ;
extern void count(int) ;
extern void flash(int) ;
extern void hexer(int) ;
extern void splat(int) ;
extern void cmdShell(int) ;



#endif
