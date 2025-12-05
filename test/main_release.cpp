#include <Arduino.h>
#include "task_core0.h"
#include "task_core1.h"
#include "encoder.h"
#include "pwm_driver.h"
#include "types.h"

void setup()
{
    Serial.begin(115200);

    encoder_init();
    pwm_init();

    startTaskCore0();   // Core 0 → encoder
    startTaskCore1();   // Core 1 → PI + PWM
}

void loop() {}
