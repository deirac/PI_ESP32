#pragma once
#include <stdint.h>

// ---------------------------------------------------------
// Estado completo del encoder para monitoreo y diagnóstico
// ---------------------------------------------------------
typedef struct {
    // Datos de velocidad
    float rpm_instant;          // RPM instantánea
    float rpm_filtered;         // RPM filtrada
    
    // Datos temporales
    uint32_t last_pulse_us;     // Tiempo último pulso (μs)
    uint32_t pulse_interval_us; // Intervalo entre pulsos (μs)
    uint32_t last_valid_time;   // Último tiempo válido (μs)
    
    // Estadísticas
    uint32_t total_pulses;      // Pulsos totales
    uint32_t pulse_count_1s;    // Pulsos en último segundo
    uint32_t error_count;       // Contador de errores
    float frequency_hz;         // Frecuencia actual (Hz)
    
    // Estado del sistema
    bool signal_ok;             // ¿Señal presente?
    bool timeout_active;        // ¿Timeout activo?
    
    // Configuración
    uint32_t ppr;               // Pulsos por revolución
    uint32_t min_interval_us;   // Intervalo mínimo
    uint32_t max_interval_us;   // Intervalo máximo
    uint32_t timeout_ms;        // Timeout para parada
} EncoderState;

// ---------------------------------------------------------
// Controlador PI con anti-windup y límites de salida
// ---------------------------------------------------------
typedef struct {
    // Parámetros del controlador
    float kp;                   // Ganancia proporcional
    float ki;                   // Ganancia integral
    // float kd;                   // Ganancia derivativa (opcional)

    // Estado interno
    float integral;             // Acumulador integral
    // float derivative;           // Término derivativo (opcional)
    float last_error;           // Último error (para derivativo opcional)
    float output;               // Salida actual del controlador
    
    // Límites y protección
    float output_min;           // Límite inferior de salida (0.05)
    float output_max;           // Límite superior de salida (0.95)
    float integral_min;         // Límite inferior para anti-windup
    float integral_max;         // Límite superior para anti-windup
    
    // Estadísticas
    uint32_t update_count;      // Número de actualizaciones
    float max_error;            // Error máximo registrado
    float avg_error;            // Error promedio
} PIController;

// ---------------------------------------------------------
// Datos compartidos para sistema completo
// ---------------------------------------------------------
typedef struct {
    // Datos del motor
    float current_rpm;          // RPM actual (filtrada)
    float target_rpm;           // RPM objetivo (setpoint)
    float duty_cycle;           // Duty cycle actual (0.0-1.0)
    
    // Estado del sistema
    bool motor_running;         // ¿Motor en funcionamiento?
    bool system_ready;          // ¿Sistema listo?
    uint32_t runtime_ms;        // Tiempo de ejecución (ms)
    
    // Rendimiento
    float power_estimate;       // Estimación de potencia (W)
    float efficiency;           // Eficiencia estimada (%)
} MotorSystemState;

// ---------------------------------------------------------
// Modo de operación 
// ---------------------------------------------------------
typedef enum {// Modo de operación del sistema
    MODE_IDLE = 0,
    MODE_TEST_ENCODER,
    MODE_TEST_PWM,
    MODE_TEST_PI,
    MODE_CALIBRATION,
    MODE_FORWARD
} SystemMode;