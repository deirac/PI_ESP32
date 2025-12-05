#include "pi_controller.h"

void pi_init(PIController *pi, float kp, float ki, float out_min, float out_max)
{
    pi->kp = kp;
    pi->ki = ki;

    pi->integral = 0.0f;
    pi->output   = 0.0f;

    pi->out_min = out_min;
    pi->out_max = out_max;
}

// -------------------------------------------------------
// PI con anti-windup
// setpoint     → referencia de RPM
// measurement  → RPM medidas
// dt           → tiempo en segundos
// -------------------------------------------------------
float pi_compute(PIController *pi, float setpoint, float measurement, float dt)
{
    float error = setpoint - measurement;

    // Parte proporcional
    float P = pi->kp * error;

    // Integración
    pi->integral += pi->ki * error * dt;
    
    // Anti-windup: recorte de la integral
    if (pi->integral > pi->out_max) pi->integral = pi->out_max;
    if (pi->integral < pi->out_min) pi->integral = pi->out_min;

    float I = pi->integral;
    // Salida PI
    //P = Kp * error
    //I = I + Ki * error * dt
    //u = P + I
    float output = P + I;

    // Saturación final (límite del PWM)
    if (output > pi->out_max) output = pi->out_max;
    if (output < pi->out_min) output = pi->out_min;

    pi->output = output;
    return output;
}
