#pragma once
#include <Arduino.h>
#include "types.h"


void pi_init(PIController *pi, float kp, float ki, float out_min, float out_max);
void pi_reset(PIController *pi);
float pi_compute(PIController *pi, float setpoint, float measurement, float dt);
