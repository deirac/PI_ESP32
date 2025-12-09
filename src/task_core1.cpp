#include <Arduino.h>
#include "task_core1.h"
#include "pi_controller.h"
#include "pwm_driver.h"
#include "config.h"
#include "types.h"

// Variables globales
extern volatile MotorSystemState motorData;

void taskCore1(void *parameter)
{
    PIController pi;
    pi_init(&pi, PI_KP, PI_KI, DUTY_MIN, DUTY_MAX);

    uint32_t last_time = micros();

    while (true)
    {
        uint32_t now = micros();
        float dt = (now - last_time) / 1e6;
        last_time = now;

        // Medición actual
        float rpm_measured = motorData.current_rpm;

        // Control PI
        float duty = pi_compute(&pi, motorData.target_rpm, rpm_measured, dt);

        // Aplicar al PWM
        pwm_set_duty(duty);

        // Ciclo de control → cada 1 ms
        vTaskDelay(1);
    }
}

void startTaskCore1()
{
    xTaskCreatePinnedToCore(
        taskCore1,
        "TaskCore1",
        6000,     // más stack por PI
        NULL,
        2,        // prioridad mayor que Core0
        NULL,
        1         // Core 1
    );
}
