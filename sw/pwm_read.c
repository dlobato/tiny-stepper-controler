#include "pwm_read.h"
#include <avr/interrupt.h>
#include <util/atomic.h>

//timer1 lsb is ~16us (16cycles) ->  elapsed_us = (timer1_value << 4)
volatile uint8_t _tcnt1 = 0;

void init_pwm_read(void){
	//init INT0 interrupt on rising_edge
	GIMSK |= _BV(INT0); // Enable INT0
	MCUCR = _BV(ISC00);// Trigger INT0 any logical change

	//timer1
	TCCR1 = 0;//stop timer1
	TCNT1 = 0;//reset timer1
	_tcnt1 = 0;

	//DDRB |= _BV(DEBUGPIN);
}

uint8_t pulse_width(){
	return _tcnt1;
}

uint16_t pulse_width_us(void){
	uint16_t width_us = (uint16_t)_tcnt1;
	width_us <<= 4;
	return width_us;
}

//STATES: invalid signal, rising edge detected, falling edge detected
ISR(INT0_vect){
	if ( (PINB & _BV(INT0PIN)) != 0){//rising_edge
		//starts timer1 with prescaler 16 -> overflows after aprox 4ms  -> if overflows invalid signal
		TCCR1 = 0;//stop timer1
		GTCCR |= _BV(PSR1);//reset timer1 prescaler counter
		TCNT1 = 0;//reset timer1
		TCCR1 = _BV(CS12) | _BV(CS10);;//start timer1 prescaler 16
		TIMSK |= _BV(TOIE1);//enable timer1 overflow interrupt
		//PORTB |= _BV(DEBUGPIN);
	}else{//falling_edge
		_tcnt1 = TCNT1;

		//if there was overflow timer1 cnt is 0, do nothing
		if (_tcnt1 != 0){
			//starts timer1 with prescaler 256 -> overflows after aprox 65ms (65536 cycles) or 3 frames at 50Hz -> if overflows invalid signal
			TCCR1 = 0;//stop timer1
			GTCCR |= _BV(PSR1);//reset timer1 prescaler counter
			TCNT1 = 0;//reset timer1
			TCCR1 = _BV(CS13) | _BV(CS10);//start timer1 prescaler 256
			TIMSK |= _BV(TOIE1);//enable timer1 overflow interrupt
		}
		//PORTB &= ~_BV(DEBUGPIN);
	}
	//PINB |= _BV(DEBUGPIN);
}

ISR(TIMER1_OVF_vect){
	//if timer1 overflows, either after rising edge (signal out of bounds) or falling edge (lost signal)
	_tcnt1 = 0;
	TCCR1 = 0;//stop timer1
	TCNT1 = 0;//reset timer1
}
