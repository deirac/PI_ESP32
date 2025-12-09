#pragma once
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "types.h"


// ============================================
// TAREA DEL CORE 0: ENCODER + CONTROLADOR PI
// ============================================


void initCore0();    // Inicializa periféricos del core 0
void startTaskCore0();   // Tarea encargada del encoder
void stopTaskCore0();    // Detiene la tarea del core 0


float getCurrentRPM();    // Devuelve la RPM actual medida
float getTargetRPM();     // Devuelve la RPM objetivo
float getPIOutput();     // Devuelve la salida actual del controlador PI
SystemMode getSystemMode(); // Devuelve el modo actual del sistema

void setTargetRPM(float rpm); // Establece la RPM objetivo
void setSystemMode(SystemMode mode); // Establece el modo del sistema
void setPIGains(float kp, float ki); // Establece las ganancias del controlador PI
void resetPIController(); // Reinicia el estado del controlador PI
