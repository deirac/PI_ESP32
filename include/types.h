#pragma once
#include <stdint.h>

// ---------------------------------------------------------
// Estado del encoder: RPM y tiempo del último pulso
// ---------------------------------------------------------
typedef struct {
    float rpm;
    uint32_t lastPulseMicros;
} EncoderState;

// ---------------------------------------------------------
// Controlador PI con anti-windup y límites de salida
// ---------------------------------------------------------
typedef struct {
    float kp;
    float ki;

    float integral;
    float output;

    float out_min;     // límite inferior (0.50)
    float out_max;     // límite superior (0.95)
} PIController;

// ---------------------------------------------------------
// Datos compartidos entre cores
// ---------------------------------------------------------
typedef struct {
    float rpm;         // medición del Core0
    float setpoint;    // referencia establecida
} MotorData;
