#include "encoder.h"
#include "types.h"
#include "config.h"

// ============================================
// VARIABLES GLOBALES VOLÁTILES (ISR)
// ============================================
volatile float current_rpm =            0.0f;         // RPM instantánea
volatile uint32_t last_pulse_time =     0;            // μs del último pulso
volatile uint32_t pulse_interval =      0;            // μs entre pulsos
volatile uint32_t last_valid_time =     0;            // Para timeout
volatile uint32_t total_pulses =        0;            // Contador total de pulsos
volatile uint32_t error_count =         0;            // Contador de errores
// ============================================

// ============================================
// BUFFER PARA FILTRADO DE RPM (MEDIA MÓVIL)
// ============================================
static float rpm_buffer[20];                   // Buffer circular para RPM
static uint8_t buffer_index =           0;     // Índice actual en el buffer
static bool buffer_filled =             false; // Indica si el buffer está lleno
// ============================================

// ============================================
// VARIABLES PARA CÁLCULO DE FRECUENCIA
// ============================================
static uint32_t pulse_count_1s =        0;      // Pulsos en el último segundo
static uint32_t last_pulse_count_time = 0;      // Último tiempo de conteo
// ============================================

// ============================================
// MUTEX PARA PROTECCIÓN DE DATOS COMPARTIDOS
// ============================================
// Mutex para secciones críticas
// Configurado para uso en ISR
// Permite proteger variables compartidas
portMUX_TYPE encoder_mutex =            portMUX_INITIALIZER_UNLOCKED; 
// ============================================


// ============================================
// ISR DEL ENCODER
// ============================================
void IRAM_ATTR encoder_isr() {
    uint32_t now = micros();
    uint32_t dt;
    
    portENTER_CRITICAL_ISR(&encoder_mutex);
    
    // Incrementar contador total
    total_pulses++;
    
    // Calcular intervalo si no es el primer pulso
    if (last_pulse_time > 0) {
        dt = now - last_pulse_time;
        
        // Filtrar intervalos válidos
        if (dt >= ENCODER_MIN_DT_US && dt <= ENCODER_MAX_DT_US) {
            pulse_interval = dt;
            
            // Cálculo de RPM: (60e6/PPR)/dt
            // Para PPR=2000: 30e3 / dt 
            // Ej: dt=100μs → RPM=30e3/100 = 300 RPM
            // Rango típico: 17.14μs (1750 RPM) a 1e6μs (0.03 RPM)
            current_rpm = 30e3f / (float)dt;
            
            last_valid_time = now;  // Resetear timeout
        } else {
            // Intervalo inválido
            error_count++;
        }
    }
    
    // Actualizar tiempo del último pulso
    last_pulse_time = now;
    
    // Actualizar contador para estadísticas de 1 segundo
    uint32_t current_time = millis();
    if (current_time - last_pulse_count_time >= 1000) {
        pulse_count_1s = 0;
        last_pulse_count_time = current_time;
    }
    pulse_count_1s++;
    
    portEXIT_CRITICAL_ISR(&encoder_mutex);
}

// ============================================
// FUNCIONES PRINCIPALES DEL ENCODER
// ============================================

void encoder_init() {
    // Configurar pin con pull-up interno
    pinMode(ENCODER_PIN_B, INPUT_PULLUP);
    
    // Configurar interrupción en flanco de subida
    attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_B), 
                    encoder_isr, 
                    RISING);
    
    // Inicializar variables
    encoder_reset();
    
    Serial.println("[ENCODER] Inicializado en pin " + String(ENCODER_PIN_B));
}

float encoder_get_rpm() {
    float rpm;
    uint32_t now;
    
    portENTER_CRITICAL(&encoder_mutex);
    
    // Verificar timeout
    now = micros();
    if ((now - last_valid_time) > (ENCODER_TIMEOUT_MS * 1000)) {
        current_rpm = 0.0f;  // Motor detenido
    }
    
    rpm = current_rpm;
    
    portEXIT_CRITICAL(&encoder_mutex);
    
    return rpm;
}

float encoder_get_filtered_rpm(uint8_t samples) {
    // Limitar el número de muestras
    if (samples > 20) samples = 20;
    if (samples < 1) samples = 1;
    
    // Obtener RPM actual
    float current = encoder_get_rpm();
    
    // Agregar al buffer
    rpm_buffer[buffer_index] = current;
    buffer_index = (buffer_index + 1) % samples;
    
    // Marcar buffer como lleno
    if (!buffer_filled && buffer_index == 0) {
        buffer_filled = true;
    }
    
    // Calcular promedio
    float sum = 0.0f;
    uint8_t count = buffer_filled ? samples : buffer_index;
    
    for (uint8_t i = 0; i < count; i++) {
        sum += rpm_buffer[i];
    }
    
    return (count > 0) ? (sum / count) : 0.0f;
}

// ============================================
// FUNCIONES DE MONITOREO Y DIAGNÓSTICO
// ============================================

void encoder_get_state(EncoderState* state) {
    if (state == NULL) return;
    
    portENTER_CRITICAL(&encoder_mutex);
    
    // Copiar datos actuales
    state->rpm_instant = current_rpm;
    state->last_pulse_us = last_pulse_time;
    state->pulse_interval_us = pulse_interval;
    state->last_valid_time = last_valid_time;
    state->total_pulses = total_pulses;
    state->pulse_count_1s = pulse_count_1s;
    state->error_count = error_count;
    
    // Calcular frecuencia
    state->frequency_hz = (pulse_interval > 0) ? (1e6f / pulse_interval) : 0.0f;
    
    // Verificar timeout
    uint32_t now = micros();
    state->timeout_active = ((now - last_valid_time) > (ENCODER_TIMEOUT_MS * 1000));
    state->signal_ok = !state->timeout_active;
    
    // Configuración
    state->ppr = ENCODER_PPR;
    state->min_interval_us = ENCODER_MIN_DT_US;
    state->max_interval_us = ENCODER_MAX_DT_US;
    state->timeout_ms = ENCODER_TIMEOUT_MS;
    
    portEXIT_CRITICAL(&encoder_mutex);
    
    // Calcular RPM filtrada
    state->rpm_filtered = encoder_get_filtered_rpm(5);
}

void encoder_reset() {
    portENTER_CRITICAL(&encoder_mutex);
    
    last_pulse_time = 0;
    pulse_interval = 0;
    current_rpm = 0.0f;
    last_valid_time = micros();
    total_pulses = 0;
    error_count = 0;
    pulse_count_1s = 0;
    last_pulse_count_time = millis();
    
    // Resetear buffer de media móvil
    for (int i = 0; i < 20; i++) {
        rpm_buffer[i] = 0.0f;
    }
    buffer_index = 0;
    buffer_filled = false;
    
    portEXIT_CRITICAL(&encoder_mutex);
}

bool encoder_signal_ok() {
    uint32_t now;
    bool signal_ok;
    
    portENTER_CRITICAL(&encoder_mutex);
    now = micros();
    signal_ok = ((now - last_valid_time) <= (ENCODER_TIMEOUT_MS * 1000));
    portEXIT_CRITICAL(&encoder_mutex);
    
    return signal_ok;
}

void encoder_debug_info() {
    static uint32_t last_debug_time = 0;
    uint32_t now = millis();
    
    if (now - last_debug_time >= 1000) {  // Cada segundo
        last_debug_time = now;
        
        EncoderState state;
        encoder_get_state(&state);
        
        Serial.print("[ENCODER] RPM: ");
        Serial.print(state.rpm_filtered, 1);
        Serial.print(" | Freq: ");
        Serial.print(state.frequency_hz, 0);
        Serial.print(" Hz | Pulses/s: ");
        Serial.print(state.pulse_count_1s);
        Serial.print(" | Signal: ");
        Serial.println(state.signal_ok ? "OK" : "NO");
    }
}

void encoder_print_detailed_state() {
    EncoderState state;
    encoder_get_state(&state);
    
    Serial.println("\n╔══════════════════════════════════╗");
    Serial.println("║     ESTADO DETALLADO ENCODER     ║");
    Serial.println("╠══════════════════════════════════╣");
    Serial.printf("║ RPM instantáneo: %6.1f RPM     ║\n", state.rpm_instant);
    Serial.printf("║ RPM filtrado:    %6.1f RPM     ║\n", state.rpm_filtered);
    Serial.printf("║ Frecuencia:      %6.0f Hz      ║\n", state.frequency_hz);
    Serial.printf("║ Intervalo:       %6u μs       ║\n", state.pulse_interval_us);
    Serial.printf("║ Pulsos totales:  %6u          ║\n", state.total_pulses);
    Serial.printf("║ Pulsos/s:        %6u          ║\n", state.pulse_count_1s);
    Serial.printf("║ Errores:         %6u          ║\n", state.error_count);
    Serial.println("╠──────────────────────────────────╣");
    Serial.printf("║ Señal:           %-14s ║\n", state.signal_ok ? "OK" : "FALLA");
    Serial.printf("║ Timeout:         %-14s ║\n", state.timeout_active ? "ACTIVO" : "INACTIVO");
    Serial.println("╚══════════════════════════════════╝\n");
}

uint32_t encoder_get_total_pulses() {
    uint32_t pulses;
    
    portENTER_CRITICAL(&encoder_mutex);
    pulses = total_pulses;
    portEXIT_CRITICAL(&encoder_mutex);
    
    return pulses;
}

float encoder_get_frequency_hz() {
    uint32_t interval;
    
    portENTER_CRITICAL(&encoder_mutex);
    interval = pulse_interval;
    portEXIT_CRITICAL(&encoder_mutex);
    
    return (interval > 0) ? (1e6f / interval) : 0.0f;
}

float encoder_get_angular_velocity() {
    // Convertir RPM a rad/s: ω = RPM × (2π/60)
    float rpm = encoder_get_filtered_rpm(5);
    return rpm * 0.10472f;  // 2π/60 ≈ 0.10472
}

// ============================================
// FUNCIONES DE CALIBRACIÓN Y PRECISIÓN
// ============================================

void encoder_calibrate_zero() {
    // Función para calibración manual del cero
    // En una implementación real, esto podría:
    // 1. Esperar a que el motor esté detenido
    // 2. Resetear contadores
    // 3. Ajustar offset si es necesario
    
    Serial.println("[ENCODER] Iniciando calibración...");
    Serial.println("Asegúrate de que el motor esté completamente detenido.");
    
    delay(2000);  // Esperar estabilización
    
    encoder_reset();
    
    Serial.println("[ENCODER] Calibración completada.");
    Serial.println("Contadores reseteados a cero.");
}

float encoder_get_accuracy() {
    // Calcular precisión estimada basada en errores
    // Porcentaje de pulsos válidos vs totales
    
    uint32_t total, errors;
    
    portENTER_CRITICAL(&encoder_mutex);
    total = total_pulses;
    errors = error_count;
    portEXIT_CRITICAL(&encoder_mutex);
    
    if (total == 0) return 100.0f;  // Sin datos, asumir 100%
    
    float accuracy = 100.0f * (1.0f - (float)errors / total);
    return (accuracy > 0.0f) ? accuracy : 0.0f;
}

// ============================================
// FUNCIÓN DE TEST AUTOMÁTICO
// ============================================

void encoder_run_self_test() {
    Serial.println("\n[ENCODER] Iniciando autoprueba...");
    
    // 1. Verificar pin configurado
    int pin_state = digitalRead(ENCODER_PIN_B);
    Serial.print("1. Estado pin " + String(ENCODER_PIN_B) + ": ");
    Serial.println(pin_state == HIGH ? "HIGH" : "LOW");
    
    // 2. Verificar interrupción configurada
    bool interrupt_ok = (digitalPinToInterrupt(ENCODER_PIN_B) != NOT_AN_INTERRUPT);
    Serial.print("2. Interrupción configurada: ");
    Serial.println(interrupt_ok ? "OK" : "FALLO");
    
    // 3. Verificar señal actual
    bool signal = encoder_signal_ok();
    Serial.print("3. Señal del encoder: ");
    Serial.println(signal ? "PRESENTE" : "AUSENTE");
    
    // 4. Verificar RPM actual
    float rpm = encoder_get_rpm();
    Serial.print("4. RPM actual: ");
    Serial.print(rpm, 1);
    Serial.println(" RPM");
    
    // 5. Verificar precisión
    float accuracy = encoder_get_accuracy();
    Serial.print("5. Precisión estimada: ");
    Serial.print(accuracy, 1);
    Serial.println("%");
    
    Serial.println("[ENCODER] Autoprueba completada.\n");
}