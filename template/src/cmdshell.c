#include "inc/rtos.h"

// This is the command line interface (CLI)
// You must have a Serial Terminal running (eg TeraTerm) to diaply and pass keypresses to this function
// A readln() helper function is availble to read the keypresses into a buffer and return when teh Enter key is pressed.
//		void readln(char *line, int size)  line is the buffer, size ensures that you do not overrun past the size of your buffer

// add_Task wants that as:
// add_Task(&flash, /*arg=*/ 3, "flash", /*priority=*/ 1);
// Look at the prototype:
// extern int add_Task(void (*Task)(int), uint32_t arg, const char *name, uint8_t priority);
// Parameter	Meaning
// &flash | function to run (address of the task)
// arg | goes into R0 → becomes pos / numflash
// "flash" | copied into TCB.name (what pt prints)
// priority | stored in the TCB (RR ignores it until Part 2)

void printTasks(void)
{
    TCB_t *t;

    DISABLE_INT(); // list is shared with the scheduler - t->next could become garbage 

    t = Task_List;
    while( t != NULL)
    {
        printf("id: %u \n name: %s \n priority: %u\n", t->id, t->name, t->priority);
        t = t->next;
    }

    ENABLE_INT();

}

void cmdShell(int dummy) {		// the dummy int is to keep consistent with the other tasks

    char line[30];

    while(1)
    {
        printf("🤙🐚 # "); // 📞
        readln(line, sizeof(line));

        char command[30];
        int num1, num2;
        int n;

        n = sscanf(line, "%s %i %i", command, &num1 ,&num2); // return number of successfully matched inputs

        if(n < 1) continue;

        if(n == 1 && strcmp( command, "pt") == 0)
        {
            printTasks();
        }

        else if(n == 2 && strcmp(command, "rt") == 0)
        {
            remove_Task(num1);
        }

        else if(n == 1 && strcmp(command, "sd") == 0)
        {
            for(int i=0;i<8;i++) display_buffer[i]=0; // clear screen
            printf("bye bye 😿\n");
            DISABLE_INT(); // stop systick
            while(1) {}
        }

        else if(n == 3 && strcmp(command, "blink") == 0)
        {
            add_Task(&blink, num1, "blink", num2);
        }

        else if(n == 3 && strcmp(command, "count") == 0)
        {
            add_Task(&count, num1, "count", num2);
        }

        else if(n == 3 && strcmp(command, "flash") == 0)
        {
            add_Task(&flash, num1, "flash", num2);
        }

        else if(n == 3 && strcmp(command, "hexer") == 0)
        {
            add_Task(&hexer, num1, "hexer", num2);
        }

        else if(n == 3 && strcmp(command, "splat") == 0)
        {
            add_Task(&splat, num1, "splat", num2);
        }

        else
        {
            printf("unknown command\n");
        }
    }
}
