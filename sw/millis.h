/* Name: millis.h
* Author: David Lobato <dav.lobato@gmail.com>
* Creation Date: 2016-07-01
* Copyright: (c) 2016 David Lobato
* License: GPLv3 ?
*/

#ifndef MILLIS_H
#define MILLIS_H

#include <inttypes.h>

// configured for F_CPU 1MHz (8Mhz clock , div 8)
// 0 = STOP (Timer not counting)
// 1 = CLOCK      tics= 1MHz
// 2 = CLOCK/8    tics= 125kHz
// 3 = CLOCK/64   tics= 15625Hz
// 4 = CLOCK/256  tics= 1953.125Hz
// 5 = CLOCK/1024 tics= 244.141Hz
#define TIMER_CLK_STOP			0x00	///< Timer Stopped
#define TIMER_CLK_DIV1			0x01	///< Timer clocked at F_CPU
#define TIMER_CLK_DIV8			0x02	///< Timer clocked at F_CPU/8
#define TIMER_CLK_DIV64			0x03	///< Timer clocked at F_CPU/64
#define TIMER_CLK_DIV256		0x04	///< Timer clocked at F_CPU/256
#define TIMER_CLK_DIV1024		0x05	///< Timer clocked at F_CPU/1024


#define TIMER0_PRESCALER TIMER_CLK_DIV8
#define TIMER0_PRESCALER_FACTOR 8
#define SYSTICK_DELAY_MS 1
#define TIMER0_COMPARE_COUNT 125 // ( SYSTICK_DELAY_MS*10e-3 * (F_CPU/TIMER0_PRESCALER_FACTOR) ) - 1 [TIMER0_COMPARE_COUNT is 8bit!]

// scheduler functions
void init_timer0(void);
uint32_t millis(void);


#endif //MILLIS_H
