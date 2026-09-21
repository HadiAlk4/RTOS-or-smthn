#include "inc/rtos.h"

// (1) Quantum event fires interrupt (Systick interrupt)
// (2)The context switch saves the current processor state to your current TCB
// (3) The context switch calls your scheduler to choose a new task and for you to assign CurrentTCB to the new task
// (4) The context switch load the context of the new task from teh current TCB onto the processor

TCB_t *PreemptTarget = NULL; // Pointer to the task that will be preempted

void scheduler(void)
{
    DISABLE_INT();
    if (Task_List == NULL) {
        CurrentTCB = NULL;
        ENABLE_INT();
        return;
    }
    if (PreemptTarget != NULL) {
        CurrentTCB = PreemptTarget;
        PreemptTarget = NULL;
        ENABLE_INT();
        return;
    }
    TCB_t *next_task = (CurrentTCB && CurrentTCB->next)
                       ? CurrentTCB->next
                       : Task_List;
    CurrentTCB = next_task;
    ENABLE_INT();
}


