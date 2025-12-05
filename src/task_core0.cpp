#include <Arduino.h>
#include "task_core0.h"
#include "encoder.h"
#include "config.h"
#include "types.h"

// Variable global accesible desde otros módulos
volatile MotorData motorData;

void taskCore0(void *parameter)
{
    unsigned long last_time = micros();

    while (true)
    {
        // Tiempo entre lecturas (usamos 1 ms)
        unsigned long now = micros();
        float dt = (now - last_time) / 1e6;
        last_time = now;

        // Leer pulsos y convertir a RPM
        motorData.rpm = encoder_get_rpm(dt);

        // Puedes añadir filtros si quieres:
        // motorData.rpm = lowpass(motorData.rpm);

        vTaskDelay(1);  // 1 ms
    }
}

void startTaskCore0()
{
    xTaskCreatePinnedToCore(
        taskCore0,
        "TaskCore0",
        4000,
        NULL,
        1,        // baja prioridad
        NULL,
        0         // Core 0
    );
}
