#include "pwm_read.h"
#include <avr/interrupt.h>
#include <util/atomic.h>

//timer1 lsb is ~16us (16cycles) ->  elapsed_us = (timer1_value << 4)
volatile uint8_t _tcnt1 = 0;

void init_pwm_read(void){
	//init INT0 interrupt on rising_edge
	MCUCR = _BV(ISC01) | _BV(ISC00);// Trigger INT0 on rising_edge
	GIMSK |= _BV(INT0); // Enable INT0

	//timer1
	TCCR1 = 0;//stop timer1
	_tcnt1 = 0;
}

uint8_t pulse_width(){
	return _tcnt1;
}

uint16_t pulse_width_us(void){
	uint16_t width_us = (uint16_t)_tcnt1;
	width_us <<= 4;
	return width_us;
}

/**
* on rising edge: reset timer1 and start timer1 with prescaler 16 to measure pulse high width. Timer1 overflows
* after aprox 4ms which should be enough to read pulses between 1ms to 2ms. If timer1 overflows timer1 is stop and current pwm width is set to 0 (no signal).
* on falling edge: get timer1 counter and set current pwm width measure.
*/
ISR(INT0_vect){
	if ( (PINB & _BV(INT0PIN)) != 0){//rising_edge
		TCCR1 = 0;//stop timer1
		GTCCR |= _BV(PSR1);//reset timer1 prescaler counter
		TCNT1 = 0;//reset timer1
		TCCR1 = _BV(CS12) | _BV(CS10);//start timer1 prescaler 16

		MCUCR = _BV(ISC01);// Trigger INT0 on falling_edge
	}else{//falling_edge
		_tcnt1 = TCNT1;

		TCCR1 = 0;//stop timer1
		GTCCR |= _BV(PSR1);//reset timer1 prescaler counter
		TCNT1 = 0;//reset timer1
		TCCR1 = _BV(CS13) | _BV(CS10);//start timer1 prescaler 256

		MCUCR = _BV(ISC01) | _BV(ISC00);// Trigger INT0 on rising_edge
	}
	TIMSK |= _BV(TOIE1);//enable timer1 overflow interrupt
}

ISR(TIMER1_OVF_vect){
	//reset and wait for new pulse
	init_pwm_read();
}
