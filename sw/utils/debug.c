#include "debug.h"

int debug_putchar(char c, FILE *stream){
	dbg_putchar(c);
	return 0;
}

FILE debug_stdout = FDEV_SETUP_STREAM(debug_putchar, NULL, _FDEV_SETUP_WRITE);
