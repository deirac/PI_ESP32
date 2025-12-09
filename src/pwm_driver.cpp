#include "pwm_driver.h"
#include "config.h"
#include <driver/mcpwm.h>


//==============================
// PWM Initialization
//==============================
void pwm_init() {// Inicializa el PWM con configuración predeterminada
    // Configuración de pines PWM
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, PWM_H_PIN);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, PWM_L_PIN);

    // Configuración del timer PWM
    mcpwm_config_t pwm_config;
    pwm_config.frequency = PWM_FREQ;
    pwm_config.cmpr_a = 0;  // Inicia en 0%
    pwm_config.cmpr_b = 0;  // Inicia en 0%
    pwm_config.counter_mode = MCPWM_UP_COUNTER;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;

    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);

    // DEADTIME
    uint32_t deadtime_ticks = (PWM_DEADTIME_NS * 80) / 1000;

    
    mcpwm_deadtime_enable(
        MCPWM_UNIT_0,
        MCPWM_TIMER_0,
        MCPWM_ACTIVE_RED_FED_FROM_PWMXA,  
        deadtime_ticks,
        deadtime_ticks
    );

    pwm_set_duty(0.0f);
}

//==============================
// Set PWM Duty Cycle
//==============================
void pwm_set_duty(float duty) { //  Establece el ciclo de trabajo del PWM (0.0 a 1.0)
    if (duty < DUTY_MIN) duty = DUTY_MIN;
    if (duty > DUTY_MAX) duty = DUTY_MAX;

    float duty_percent = duty * 100.0f;

    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, duty_percent);
    mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);

}

//==============================
// Get PWM Duty Cycle
//==============================
float pwm_get_duty() { // Obtiene el ciclo de trabajo actual del PWM (0.0 a 1.0)
    return mcpwm_get_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A) /100.0f;
}

//==============================
// Ramp PWM Duty Cycle
//==============================
void pwm_ramp_duty(float target_duty, float step, uint32_t delay_ms) { // Rampa el ciclo de trabajo del PWM hacia un objetivo
    float current_duty = pwm_get_duty();

    if (target_duty < DUTY_MIN) target_duty = DUTY_MIN;
    if (target_duty > DUTY_MAX) target_duty = DUTY_MAX;

    if (current_duty < target_duty) {
        for (float d = current_duty; d < target_duty; d += step) {
            pwm_set_duty(d);
            delay(delay_ms);
        }
    } else {
        for (float d = current_duty; d > target_duty; d -= step) {
            pwm_set_duty(d);
            delay(delay_ms);
        }
    }
    pwm_set_duty(target_duty); // Asegura que se establezca el valor exacto al final
}

//==============================
// Stop PWM
//==============================
void pwm_stop() { // Detiene el PWM estableciendo el ciclo de trabajo a 0
    pwm_set_duty(0.0f);
}