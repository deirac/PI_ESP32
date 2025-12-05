#pragma once

// Pines encoder
#define ENCODER_PIN_B 34

// Pines PWM IR2110
#define PWM_H_PIN 25
#define PWM_L_PIN 26

// Parámetros PWM
#define PWM_FREQ 20000
#define PWM_DEADTIME_NS 500

// Duty máximo/mínimo
#define DUTY_MIN 0.50
#define DUTY_MAX 0.95

// Setpoint RPM
#define DEFAULT_SETPOINT 900

// PI parámetros
#define PI_KP 0.1f
#define PI_KI 0.02f
