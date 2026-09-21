#include "inc/rtos.h"

// (1) Quantum event fires interrupt (Systick interrupt)
// (2)The context switch saves the current processor state to your current TCB
// (3) The context switch calls your scheduler to choose a new task and for you to assign CurrentTCB to the new task
// (4) The context switch load the context of the new task from teh current TCB onto the processor


void scheduler(void)
{
    TCB_t *t;

    DISABLE_INT();

    t = (TCB_t *)CurrentTCB;

    if (t != NULL) t = t->next;

    if (t == NULL) t = Task_List;

    CurrentTCB = t;

    ENABLE_INT();
}


