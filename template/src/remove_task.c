#include "inc/rtos.h"

// searches the singly linked list for that ID, 
// unlinks the node, and gives the TCB back to the heap. 
// It does not stop the CPU mid-instruction. 
// After the node is off Task_List, 
// the stock scheduler simply never picks that task again

int remove_Task(uint32_t Task_ID)
{
    TCB_t *prev = NULL ;   
    TCB_t *curr;
    DISABLE_INT():
    
    curr = Task_List;

    while(curr != NULL)
    {
        if(curr->id == Task_ID)
        {
            break;
        }
        prev = curr;
        curr = curr->next;
    }

    if(curr == NULL)
    {
        ENABLE_INT();
        return 0;
    }

    if(prev == NULL) Task_List = curr->next;
    else prev->next = curr->next;

    int was_current = (CurrentTCB == curr);

    if(was_current)
    {
        if(curr->next != NULL) CurrentTCB = curr->next;
        else CurrentTCB = Task_List;
    }

    ENABLE_INT();

    if(!was_current) free(curr);

    return Task_ID;
    
}


