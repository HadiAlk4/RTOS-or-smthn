#include "inc/rtos.h"

// This is the command line interface (CLI)
// You must have a Serial Terminal running (eg TeraTerm) to diaply and pass keypresses to this function
// A readln() helper function is availble to read the keypresses into a buffer and return when teh Enter key is pressed.
//		void readln(char *line, int size)  line is the buufer, size ensures that you do not overrun past the size of your buffer

void cmdShell(int dummy) {		// the dummy int is to keep consistent with the other tasks

}
