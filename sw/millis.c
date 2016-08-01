/* Name: millis.c
*/

#include "millis.h"
#include <avr/interrupt.h>
#include <util/atomic.h>

//millis since init
volatile uint32_t timer0_millis = 0;

void init_timer0(void){
	//init timer0
	/*
	Control Register A for Timer/Counter-0 (Timer/Counter-0 is configured using two registers: A and B)
	TCCR0A is 8 bits: [COM0A1:COM0A0:COM0B1:COM0B0:unused:unused:WGM01:WGM00]
	sets bits WGM0[1:0]=2, CTC mode
	*/
	TCCR0A = _BV(WGM01);

	/*
	Control Register B for Timer/Counter-0 (Timer/Counter-0 is configured using two registers: A and B)
	TCCR0B is 8 bits: [FOC0A:FOC0B:unused:unused:WGM02:CS02:CS01:CS00]
	CS0[2:0] timer prescaler
	*/
	TCCR0B = TIMER0_PRESCALER << CS00;

	OCR0A = TIMER0_COMPARE_COUNT;
	TCNT0 = 0;

	TIMSK |= _BV(OCIE0A);
}

uint32_t millis(){
	uint32_t m;
	ATOMIC_BLOCK(ATOMIC_FORCEON){
		m = timer0_millis;
	}
	return m;
}

ISR(TIMER0_COMPA_vect)
{
	timer0_millis += SYSTICK_DELAY_MS;
}
