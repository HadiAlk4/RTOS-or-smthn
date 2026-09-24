#include "inc/rtos.h"

// A brand-new task has never been interrupted, 
// so it has nothing to pop. Appendix B: you plant a dummy frame that looks like a saved context. 
// rtos_Start() (and later switches) then “return” into the task function with arg already in R0.
// main does CurrentTCB = Task_List after the first add, and expects id 1. Return that id, or -1 on failure (Appendix A).

uint32_t get_quantum(uint8_t priority)
{
    switch(priority)
    {
        case PRIO_HIGH: return QUANTUM_HIGH;
        case PRIO_MED: return QUANTUM_MED;
        case PRIO_LOW: return QUANTUM_LOW;
    }
    return QUANTUM_LOW;
}

int add_Task(void (*Task)(int), uint32_t arg, const char *name, uint8_t priority)
{
    TCB_t *tcb = malloc(sizeof(TCB_t));
    if(tcb == NULL)
    {
        return -1;
    }
    memset(tcb, 0, sizeof(TCB_t)); // may contain sm rubbish so zero it


    static int next_id = 1; // cant be local - static keeps the counter between calls.
    tcb->id = next_id;
    tcb->priority = priority;
    strncpy(tcb->name, name, sizeof(tcb->name) - 1); // strncpy + the - 1 leaves room for the '\0' in name[32].
    tcb->next = NULL;


    uint32_t *frame = &tcb->TCB_Stack[STACK_SIZE - 1]; // compiler is treating those type mismatches as errors

    frame[0] = 0x01000000; // PSR, Thumb
    frame[-1] = (uint32_t)Task; // PC -- compiler is treating those type mismatches as errors
    frame[-7] = arg; // R0

    tcb->Stack_Pointer = &frame[-15];
    
    // Set the quantum and ticks_left for the task
    tcb->quantum = get_quantum(priority);
    tcb->ticks_left = tcb->quantum;

    DISABLE_INT();


    if(Task_List == NULL)
    {
        Task_List = tcb;
    } else
    {
        TCB_t *iterate = Task_List;
        while(iterate->next != NULL)
        {
            iterate = iterate->next;
        }
        iterate->next = tcb;
    }

    ENABLE_INT();

    if (CurrentTCB != NULL && tcb->priority > CurrentTCB->priority) // matches priority of current task
    {
        PreemptTarget = tcb;
        CONTEXT_SWITCH();
    }

    next_id++;
    return tcb->id;
}
