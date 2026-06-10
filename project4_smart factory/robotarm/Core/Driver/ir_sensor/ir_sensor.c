/*
 * ir_sensor.c
 */

#include "ir_sensor.h"
#include "main.h"   // ★ IR_SENSOR_Pin 매크로 사용

volatile uint8_t ir_detected = 0;

void IR_Sensor_Init(void) {
    // CubeMX에서 GPIO EXTI 설정 완료된 상태
    // 추가 초기화 불필요
}

uint8_t IR_Sensor_IsDetected(void) {
    return ir_detected;
}

void IR_Sensor_ClearFlag(void) {
    ir_detected = 0;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == IR_SENSOR_Pin) {   // ★ PA5 → IR_SENSOR_Pin (PB0)
        ir_detected = 1;
    }
}
