#include "pwm_driver.h"
#include "config.h"
#include <driver/mcpwm.h>

void pwm_init() {
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

    // DEADTIME - probar diferentes opciones
    uint32_t deadtime_ticks = (PWM_DEADTIME_NS * 80) / 1000;

    // PRIMERA OPCIÓN: Intentar con constantes básicas
    mcpwm_deadtime_enable(
        MCPWM_UNIT_0,
        MCPWM_TIMER_0,
        MCPWM_ACTIVE_HIGH_MODE,  // Esta debería existir
        deadtime_ticks,
        deadtime_ticks
    );
}

void pwm_set_duty(float duty) {
    if (duty < DUTY_MIN) duty = DUTY_MIN;
    if (duty > DUTY_MAX) duty = DUTY_MAX;

    float duty_percent = duty * 100.0f;

    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, duty_percent);
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, duty_percent);

    mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
    mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, MCPWM_DUTY_MODE_0);
}