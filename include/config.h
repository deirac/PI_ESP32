#pragma once

// ============================================
// MOTOR BALDOR RELIANCE CD6215
// ============================================
#define MOTOR_MAX_RPM          1750   // RPM máximo del motor
#define DEFAULT_SETPOINT       900  // RPM de referencia inicial


// ============================================
// ENCODER E6B2-CWZ6C CONFIGURATION
// ============================================
#define ENCODER_PIN_B          34      // Pin GPIO para canal B
#define ENCODER_PPR            2e3   // 2000 pulsos por revolución
#define ENCODER_MIN_DT_US      (60e6/ ((MOTOR_MAX_RPM * ENCODER_PPR) ))  // ~17μs
#define ENCODER_MAX_DT_US      1e6  // 1 s = 1,000,000 μs
#define ENCODER_TIMEOUT_MS     100      // Timeout para velocidad cero


// ============================================
// PWM CONFIGURATION
// ============================================
#define PWM_H_PIN               25
#define PWM_L_PIN               26
#define PWM_FREQ                20e3  // 20 kHz
#define PWM_DEADTIME_NS         100 // 100 ns
#define DUTY_MIN                0.05f // 5%
#define DUTY_MAX                0.95f // 95%


// ============================================
// PI CONTROLLER
// ============================================
#define PI_KP                   (1.0f/10.0f)  // Kp = 0.1
#define PI_KI                   (2.0f/100.0f) // Ki = 0.02