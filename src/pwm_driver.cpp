#include "pwm_driver.h"
#include "config.h"
#include <driver/mcpwm.h>

// ----------------------------
//  Inicialización del PWM
// ----------------------------
void pwm_init() {

    // Configurar los pines para MCPWM
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, PWM_H_PIN); // PWM High-side
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, PWM_L_PIN); // PWM Low-side

    // Configurar MCPWM
    mcpwm_config_t pwm_config;
    pwm_config.frequency = PWM_FREQ;     // frecuencia en Hz
    pwm_config.cmpr_a = 50.0;            // duty inicial (A) en %
    pwm_config.cmpr_b = 50.0;            // duty inicial (B) en %
    pwm_config.counter_mode = MCPWM_UP_COUNTER;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0; // modo normal

    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);

    // -------------------------
    //  Configurar DEADTIME
    // -------------------------
    // PWM_A (0A) es el HIGH SIDE
    // PWM_B (0B) es el LOW SIDE

    mcpwm_deadtime_type_t dt_type = MCPWM_BYPASS_RED_FED;  // insertar en ambos flancos

    mcpwm_deadtime_enable(
        MCPWM_UNIT_0,
        MCPWM_TIMER_0,
        dt_type,
        PWM_DEADTIME_NS,   // Rising Edge Deadtime
        PWM_DEADTIME_NS    // Falling Edge Deadtime
    );
}

// ----------------------------
//   Cambiar duty entre 0 y 1
// ----------------------------
void pwm_set_duty(float duty) {

    // Limitar duty dentro de los rangos definidos en config.h
    if (duty < DUTY_MIN) duty = DUTY_MIN;
    if (duty > DUTY_MAX) duty = DUTY_MAX;

    float duty_percent = duty * 100.0f;

    // Canal A = señal principal
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, duty_percent);

    // Canal B = complementario (invertido por hardware con deadtime)
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, duty_percent);

    // IMPORTANTE: actualizar también modo duty
    mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0,
                        MCPWM_OPR_A, MCPWM_DUTY_MODE_0);

    mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0,
                        MCPWM_OPR_B, MCPWM_DUTY_MODE_0);
}
