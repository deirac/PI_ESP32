#include "task_core0.h"
#include "encoder.h"
#include "pi_controller.h"
#include "config.h"


// -------------------------------------------
// Variables globales para la tarea del core 0
// -------------------------------------------
static TaskHandle_t taskCore0Handle = NULL;
static PIController pi_controller;
static float target_rpm = 0.0f;
static SystemMode current_mode = MODE_IDLE;
static MotorSystemState motor_state = {0};
static portMUX_TYPE motor_state_mutex = portMUX_INITIALIZER_UNLOCKED;
// -------------------------------------------


void taskCore0(void* parameter){
    Serial.println("[Core 0]: Task Started");

    uint32_t last_update_time = millis();

    while (1)
    {
       
    }
    
}