#pragma once
#include <Arduino.h>
#include "types.h"
#include "config.h"

void encoder_init();
float encoder_get_rpm();     // Devuelve RPM calculada
void IRAM_ATTR encoder_isr(); // ISR (opcional exponer)
