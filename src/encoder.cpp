#include "encoder.h"

// Variables compartidas (volatile + critical region)
volatile uint32_t lastMicros = 0;
volatile float currentRPM = 0.0f;

portMUX_TYPE encoderMux = portMUX_INITIALIZER_UNLOCKED;

// -----------------------------------
//    ISR: lee el canal B del encoder
// -----------------------------------
void IRAM_ATTR encoder_isr() {
    uint32_t now = micros();
    uint32_t dt;

    portENTER_CRITICAL_ISR(&encoderMux);

    dt = now - lastMicros;
    lastMicros = now;

    // Evitar división por cero y pulsos espurios
    if (dt > 0 && dt < 500000) { 
        // Fórmula RPM = (60e6 / (PPR * dt_us))
        currentRPM = (60.0f * 1000000.0f) / (2000.0f * dt);
    }

    portEXIT_CRITICAL_ISR(&encoderMux);
}

// -----------------------------------
//     Inicializar pin e interrupción
// -----------------------------------
void encoder_init() {
    pinMode(ENCODER_PIN_B, INPUT_PULLUP);

    // ISR en el flanco que prefieras
    attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_B), encoder_isr, RISING);

    lastMicros = micros();
}

// -----------------------------------
//      Devuelve la última RPM
// -----------------------------------
float encoder_get_rpm() {
    float rpm;
    portENTER_CRITICAL(&encoderMux);
    rpm = currentRPM;
    portEXIT_CRITICAL(&encoderMux);    
    return rpm;
}
