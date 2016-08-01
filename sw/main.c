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
#include "millis.h"
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

#define BLINK_DELAY_MS 1000
#define READ_PWM_DELAY_MS 20

#define PWM_MID_POINT 1500
#define PWM_MID_POINT_ERROR 50
#define PWM_MAX_POINT 2000
#define PWM_MIN_POINT 1000

#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))

//lookup table to convert from dist to center point to step delay_ms
//we are going to map dist_to_center_position range [0-500] -> distToDelay_Idx [0-8] ~ aprox /64
uint8_t distToDelay[] = { 8, 7, 6, 5, 4, 3, 2, 1 };

void setup(void){
	//config outputs
	DDRB |= _BV(LEDPIN) | _BV(ENABLEPIN) | _BV(STEPPIN) | _BV(DIRPIN);


	init_timer0();//start timer0
	init_pwm_read();

	sei();//enable global interrupts
}

int
main (void)
{
	setup();

	uint32_t now;
	uint32_t millis_last_blink = 0;
	uint32_t millis_last_pwm_read = 0;
	uint32_t millis_last_step = 0;
	uint16_t pwm_us = 0;
	uint8_t step_delay_ms;

	while(1){
		now = millis();

		if ((now - millis_last_blink) >= BLINK_DELAY_MS){
			PINB = _BV(LEDPIN);//toogle pin value
			millis_last_blink = now;
		}

		if ((now - millis_last_pwm_read) >= READ_PWM_DELAY_MS){
			pwm_us = pulse_width_us();
			millis_last_pwm_read = now;
			if ( (pwm_us < PWM_MIN_POINT || pwm_us > PWM_MAX_POINT ) ||
				 (pwm_us > (PWM_MID_POINT - PWM_MID_POINT_ERROR) && pwm_us < (PWM_MID_POINT + PWM_MID_POINT_ERROR)) ){//pwm value out of range or middle point
				PORTB |= _BV(ENABLEPIN);//disable motor
				pwm_us = 0;
			}else{
				uint16_t dist_to_center_position;
				PORTB &= ~_BV(ENABLEPIN);//enable motor
				if (pwm_us < 1500){//set direction
					PORTB |= _BV(DIRPIN);
					dist_to_center_position = pwm_us - PWM_MIN_POINT;
				}else{
					PORTB &= ~_BV(DIRPIN);
					dist_to_center_position = PWM_MAX_POINT - pwm_us;
				}
				step_delay_ms = distToDelay[(uint8_t)(dist_to_center_position>>6)];
			}
		}

		if ((pwm_us != 0) && ((now - millis_last_step) >= step_delay_ms)){//pwm_us != 0 => motor enabled
			PINB = _BV(STEPPIN);//toogle pin value
			millis_last_step = now;
		}
	}
}
