/*
 * ap_main.h
 *
 *  Created on: Apr 13, 2026
 *      Author: kccistc
 */

#ifndef APP_AP_MAIN_H_
#define APP_AP_MAIN_H_

#include "stm32f4xx_hal.h"

void App_Init(TIM_HandleTypeDef *htim2, TIM_HandleTypeDef *htim3,
              TIM_HandleTypeDef *htim4, UART_HandleTypeDef *huart2);
void App_Run(void);

#endif /* APP_AP_MAIN_H_ */
