#ifndef PWM_READ_H
#define PWM_READ_H

#include <stdint.h>

#define INT0PIN PB2

void init_pwm_read(void);
uint8_t pulse_width(void);
uint16_t pulse_width_us(void);

#endif //PWM_READ_H
