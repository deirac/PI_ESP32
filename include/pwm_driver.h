#ifndef PWM_DRIVER_H
#define PWM_DRIVER_H

#include <Arduino.h>

void pwm_init();
void pwm_set_duty(float duty);
void pwm_debug_info();  // Opcional

#endif