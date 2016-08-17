/* Name: main.c
* Project: tiny-stepper-controller
* Author: David Lobato <dav.lobato@gmail.com>
* Creation Date: 2016-07-01
* Copyright: (c) 2016 David Lobato
* License: GPLv3 ?
*/


#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include "millis.h"
#include "pwm_read.h"
#include "debug.h"


//set debugwire fuse
//-U lfuse:w:0x62:m -U hfuse:w:0x9f:m -U efuse:w:0xff:m
//set default fuses
//-U lfuse:w:0x62:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m


//PINS
#define ENABLEPIN PB0
#define STEPPIN PB3
#define DIRPIN PB4

#define READ_PWM_DELAY_MS 100 //10Hz
#define PRINT_DEBUG_INFO_DELAY 10000

#define PWM_MID_POINT 1500
#define PWM_MID_POINT_ERROR 50
#define PWM_MAX_POINT 2000
#define PWM_MIN_POINT 1000

#define max(a,b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a > _b ? _a : _b; })
#define min(a,b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a < _b ? _a : _b; })

//lookup table to convert from dist to center point to step delay_ms
//we are going to map dist_to_center_position range [0-500] -> distToDelay_Idx [0-8] ~ aprox /64
uint8_t distToDelay[8] = { 4, 4, 4, 3, 3, 2, 2, 1 };

void setup(void){
	//config outputs
	DDRB |= _BV(ENABLEPIN) | _BV(STEPPIN) | _BV(DIRPIN);

	init_timer0();
	init_pwm_read();
	debug_init();

	sei();//enable global interrupts
}

int
main (void)
{
	uint32_t now;
	uint32_t millis_last_pwm_read = 0;
	uint32_t millis_last_step = 0;
	uint32_t millis_last_print = 0;

	uint16_t pwm_us = 0;
	uint8_t step_delay_ms = 0;

	setup();

	while(1){
		now = millis();

		if ((now - millis_last_pwm_read) >= READ_PWM_DELAY_MS) {
			millis_last_pwm_read = now;

			if ((pwm_us = pulse_width_us()) > 0) {//we've got a valid signal
				//clamp value to range [PWM_MIN_POINT, PWM_MAX_POINT]
				pwm_us = max(PWM_MIN_POINT, pwm_us);
				pwm_us = min(PWM_MAX_POINT, pwm_us);

				if (pwm_us > (PWM_MID_POINT - PWM_MID_POINT_ERROR) && pwm_us < (PWM_MID_POINT + PWM_MID_POINT_ERROR)) {//mid point +- err
					PORTB |= _BV(ENABLEPIN);//disable motor
					pwm_us = 0;
				} else {
					uint16_t dist_to_center_position;
					PORTB &= ~_BV(ENABLEPIN);//enable motor
					if (pwm_us < 1500){//set direction
						PORTB |= _BV(DIRPIN);
						dist_to_center_position = PWM_MID_POINT - pwm_us;
					}else{
						PORTB &= ~_BV(DIRPIN);
						dist_to_center_position = pwm_us - PWM_MID_POINT;
					}
					step_delay_ms = distToDelay[(uint8_t)(dist_to_center_position>>6)];
				}
			}
		}

		if ((pwm_us != 0) && ((now - millis_last_step) >= step_delay_ms)){
			millis_last_step = now;
			PINB = _BV(STEPPIN);//toogle pin value
		}

		if ((now - millis_last_print) >= PRINT_DEBUG_INFO_DELAY) {
			millis_last_print = now;
			debug_print_P(PSTR("pwm_us=%u\r\n"), pulse_width_us());
		}
	}
}
