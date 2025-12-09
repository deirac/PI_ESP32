#ifndef PWM_DRIVER_H
#define PWM_DRIVER_H

#include <Arduino.h>

void pwm_init();
void pwm_set_duty(float duty);
float pwm_get_duty();
void pwm_ramp_duty(float target_duty, float step, uint32_t delay_ms);
void pwm_stop();

#endif