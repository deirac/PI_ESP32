# pi_esp32 -- Sistema de Control de Velocidad con Encoder y Controlador PI

Este proyecto implementa un sistema de control de velocidad para motor
utilizando un **ESP32**, un **encoder**, un **driver PWM**, y un
**controlador PI**.\
La arquitectura se encuentra organizada en módulos independientes y
tareas distribuidas entre los dos núcleos del ESP32 para lograr un
control eficiente, estable y escalable.

------------------------------------------------------------------------

## 📁 Estructura del Proyecto

    include/
     ├── config.h
     ├── types.h
     ├── encoder.h 
     ├── pwm_driver.h
     ├── pi_controller.h
     ├── task_core0.h
     └── task_core1.h
     
    src/
     ├── encoder.cpp 
     ├── pwm_driver.cpp
     ├── pi_controller.cpp
     ├── task_core0.cpp
     ├── task_core1.cpp
     └── main.c

------------------------------------------------------------------------

## ⚙️ Objetivo del Sistema

Regular la velocidad del motor (RPM) mediante:

-   Lectura del encoder\
-   Cálculo preciso de RPM\
-   Controlador PI con anti-windup\
-   Ajuste dinámico del PWM\
-   Tareas en paralelo en ambos núcleos del ESP32

------------------------------------------------------------------------

## 🧩 Componentes del Sistema

### 🔄 Encoder

Responsable de medir el tiempo entre pulsos para calcular RPM.

``` c
typedef struct {
    float rpm;
    uint32_t lastPulseMicros;
} EncoderState;
```

------------------------------------------------------------------------

### 🎚️ Controlador PI

``` c
typedef struct {
    float kp;
    float ki;
    float integral;
    float output;
} PIController;
```

La función principal:

``` c
float pi_compute(PIController *pi, float setpoint, float measurement, float dt);
```

------------------------------------------------------------------------

### ⚡ Driver PWM

Control de la señal PWM utilizando LEDC del ESP32.

------------------------------------------------------------------------

### 🧵 Tasks (FreeRTOS)

#### Core 0 -- Lazo de control

-   Lee el encoder\
-   Calcula RPM\
-   Ejecuta PI\
-   Actualiza el PWM

#### Core 1 -- Telemetría / Backend

------------------------------------------------------------------------

## 📌 types.h Final

``` c
#pragma once

typedef struct {
    float rpm;
    uint32_t lastPulseMicros;
} EncoderState;

typedef struct {
    float kp;
    float ki;
    float integral;
    float output;
} PIController;

typedef struct {
    float rpm;
    float setpoint;
} MotorData;
```

------------------------------------------------------------------------

## ✔️ Ventajas del Diseño

-   Modular\
-   Fácil de mantener\
-   Control robusto\
-   Aprovecha los dos núcleos\
-   Minimiza jitter en el lazo de control

------------------------------------------------------------------------

## 📌 TODO / Mejoras Futuras

-   PID completo\
-   Filtro para RPM\
-   Dashboard web\
-   MQTT / ESP-NOW\
-   Auto-tuning del PI
