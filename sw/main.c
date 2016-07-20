/* Name: main.c
* Project: tiny-stepper-controller
* Author: David Lobato <dav.lobato@gmail.com>
* Creation Date: 2016-07-01
* Copyright: (c) 2016 David Lobato
* License: GPLv3 ?
*/


#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include "scheduler.h"
#include "pwm_read.h"


//set debugwire fuse
//-U lfuse:w:0x62:m -U hfuse:w:0x9f:m -U efuse:w:0xff:m
//set default fuses
//-U lfuse:w:0x62:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m


//PINS
#define LEDPIN PB1
#define ENABLEPIN PB0
#define STEPPIN PB3
#define DIRPIN PB4

#define BLINK_DELAY 100 //unit is 1e-2s -> 1000ms
#define READ_PWM_DELAY 2 //unit is 1e-2s -> 20ms/50Hz
#define STEP_DELAY 10 //unit is 1e-2s -> 10ms

#define LOGGER_SIZE 512
uint8_t EEMEM logger[LOGGER_SIZE] = {0};
uint16_t loggerIdx = 0;
uint16_t pwm_us = 0;

enum task_rc_t task_blink(void){
	PINB = _BV(LEDPIN);//toogle pin value
	return RC_CONTINUE;
}

enum task_rc_t task_read_pwm(void){
	//PINB = _BV(LEDPIN);//toogle pin value
	pwm_us = ((uint16_t)pulse_width()) << 4;
	return RC_CONTINUE;
	//eeprom_write_byte (&logger[loggerIdx++], p);
	//return loggerIdx < LOGGER_SIZE? RC_CONTINUE: RC_DONE;
}

void step(void){
	PINB = _BV(STEPPIN);//toogle pin value
	_delay_ms(2);
	PINB = _BV(STEPPIN);//toogle pin value
	_delay_ms(2);
}

void setup(void){
	//config outputs
	DDRB |= _BV(LEDPIN) | _BV(ENABLEPIN) | _BV(STEPPIN) | _BV(DIRPIN);


	init_scheduler();//start timer0
	init_pwm_read();

	sei();//enable global interrupts
}

int
main (void)
{
	setup();
	//
	uint8_t blink_tid = add_task(task_blink, BLINK_DELAY);
	uint8_t pwm_tid = add_task(task_read_pwm, READ_PWM_DELAY);

	while(1){
		dispatch_tasks();

		if ( pwm_us == 0 || (pwm_us > 1450 && pwm_us < 1550) ){//invalid signal or middle point
			PORTB |= _BV(ENABLEPIN);//disable motor
		}else{
			PORTB &= ~_BV(ENABLEPIN);//enable motor
			if (pwm_us < 1500){//set direction
				PORTB |= _BV(DIRPIN);
			}else{
				PORTB &= ~_BV(DIRPIN);
			}

			step();
		}
	}
}
