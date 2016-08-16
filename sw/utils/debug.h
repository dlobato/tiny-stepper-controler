#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>
#include <avr/pgmspace.h>
#include "dbg_putchar.h"

extern FILE debug_stdout;

#define debug_init() dbg_tx_init()

#define debug_print(fmt, ...) \
	do { if (DEBUG) fprintf(&debug_stdout, fmt, __VA_ARGS__); } while (0)

#define debug_print_P(fmt, ...) \
	do { if (DEBUG) fprintf_P(&debug_stdout, fmt, __VA_ARGS__); } while (0)

#define debug_puts(str) \
	do { if (DEBUG) fputs(str, &debug_stdout); } while (0)

#define debug_puts_P(str) \
	do { if (DEBUG) fputs_P(str, &debug_stdout); } while (0)


#endif /*DEBUG_H*/
