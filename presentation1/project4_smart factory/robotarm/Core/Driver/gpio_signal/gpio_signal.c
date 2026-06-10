/*
 * gpio_signal.c
 */

#include "gpio_signal.h"

void GPIO_Signal_Init(void) {
    // 사용하지 않음 (UART로 통신)
}

void GPIO_Signal_SendComplete(void) {
    // 사용하지 않음 (UART DONE 메시지로 대체)
}

uint8_t GPIO_Signal_ReadCNN(void) {
    uint8_t bit0 = HAL_GPIO_ReadPin(CNN_0_GPIO_Port, CNN_0_Pin);
    uint8_t bit1 = HAL_GPIO_ReadPin(CNN_1_GPIO_Port, CNN_1_Pin);
    return (bit1 << 1) | bit0;   // 0~3 반환
}
