// Archivo: test_pwm.cpp (crea uno nuevo para pruebas)
#include <Arduino.h>
#include "pwm_driver.h"

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("Iniciando prueba de PWM...");
    pwm_init();
    
    // Prueba segura: verificar que las señales son complementarias
    Serial.println("Configurando duty cycle al 50%...");
    pwm_set_duty(0.5);
    
    Serial.println("Prueba completada.");
    Serial.println("¡USAR OSCILOSCOPIO para verificar señales antes de conectar carga!");
    Serial.println("Verificar que las señales sean complementarias y tengan deadtime.");
}

void loop() {
    // Puedes variar el duty cycle lentamente para pruebas
    static float duty = 0.0;
    static bool increasing = true;
    
    pwm_set_duty(duty);
    
    if (increasing) {
        duty += 0.001;
        if (duty >= 0.9) increasing = false;
    } else {
        duty -= 0.001;
        if (duty <= 0.1) increasing = true;
    }
    
    delay(10);
}