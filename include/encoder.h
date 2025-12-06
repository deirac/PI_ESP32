#pragma once
#include <Arduino.h>
#include "config.h"

// ============================================
// FUNCIONES PRINCIPALES DEL ENCODER
// ============================================
void encoder_init();
float encoder_get_rpm();
float encoder_get_filtered_rpm(uint8_t samples = 5);

// ============================================
// FUNCIONES DE MONITOREO Y DIAGNÓSTICO
// ============================================
void encoder_get_state(EncoderState* state);
void encoder_reset();
bool encoder_signal_ok();
void encoder_debug_info();
void encoder_print_detailed_state();
uint32_t encoder_get_total_pulses();
float encoder_get_frequency_hz();
float encoder_get_angular_velocity(); 

// ============================================
// FUNCIONES DE CALIBRACIÓN Y PRECISIÓN
// ============================================
void encoder_calibrate_zero();
float encoder_get_accuracy();

// ============================================
// ISR DEL ENCODER
// ============================================
void IRAM_ATTR encoder_isr();