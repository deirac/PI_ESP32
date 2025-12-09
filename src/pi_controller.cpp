#include "pi_controller.h"
#include "config.h"

// ===================================================
// Inicialización del controlador PI
// ===================================================
void pi_init(// Inicializa los parámetros del controlador PI
    PIController *pi, 
    float kp, 
    float ki, 
    float output_min, 
    float output_max){
    pi->kp = kp;
    pi->ki = ki;

    pi->integral = 0.0f;
    pi->output   = 0.0f;

    pi->output_min = output_min;
    pi->output_max = output_max;
}

// ===================================================
// Resetea el estado interno del controlador PI
// ===================================================
void pi_reset(PIController *pi){// Resetea el estado interno del controlador PI
    pi->integral =          0.0f;
    pi->output   =          0.0f;
    pi->last_error =        0.0f;
    pi->update_count =      0;
    pi->avg_error =         0.0f;
    pi->kp =                PI_KP;
    pi->ki =                PI_KI;
}

// -------------------------------------------------------
// PI con anti-windup
// setpoint     → referencia de RPM
// measurement  → RPM medidas
// dt           → tiempo en segundos
// -------------------------------------------------------
float pi_compute(
    PIController *pi, 
    float setpoint, 
    float measurement, 
    float dt)
{
    float error = setpoint - measurement;

    // Parte proporcional
    float P = pi->kp * error;

    // Integración
    pi->integral += pi->ki * error * dt;
    
    // Anti-windup: recorte de la integral
    if (pi->integral > pi->output_max) pi->integral = pi->output_max;
    if (pi->integral < pi->output_min) pi->integral = pi->output_min;

    float I = pi->integral;
    // Salida PI
    //P = Kp * error
    //I = I + Ki * error * dt
    //u = P + I
    float output = P + I;

    // Saturación final (límite del PWM)
    if (output > pi->output_max) output = pi->output_max;
    if (output < pi->output_min) output = pi->output_min;

    pi->output = output;
    return output;
}
