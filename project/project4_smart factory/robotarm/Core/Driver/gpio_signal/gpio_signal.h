/*
 * gpio_signal.h
 *
 *  Created on: Apr 13, 2026
 *      Author: kccistc
 */

#ifndef DRIVER_GPIO_SIGNAL_GPIO_SIGNAL_H_
#define DRIVER_GPIO_SIGNAL_GPIO_SIGNAL_H_


#include "stm32f4xx_hal.h"
#include "main.h"

void GPIO_Signal_Init(void);
void GPIO_Signal_SendComplete(void);  // STM32-2로 완료 신호 전송
uint8_t GPIO_Signal_ReadCNN(void);


#endif /* DRIVER_GPIO_SIGNAL_GPIO_SIGNAL_H_ */
