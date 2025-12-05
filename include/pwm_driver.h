#pragma once
#include <Arduino.h>

void pwm_init();
void pwm_set_duty(float duty);   // duty entre 0.0 y 1.0
