/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define MOTOR3_IN1_Pin GPIO_PIN_0
#define MOTOR3_IN1_GPIO_Port GPIOC
#define MOTOR3_IN2_Pin GPIO_PIN_1
#define MOTOR3_IN2_GPIO_Port GPIOC
#define MOTOR4_IN1_Pin GPIO_PIN_2
#define MOTOR4_IN1_GPIO_Port GPIOC
#define MOTOR4_IN2_Pin GPIO_PIN_3
#define MOTOR4_IN2_GPIO_Port GPIOC
#define MOTOR_IN1_Pin GPIO_PIN_0
#define MOTOR_IN1_GPIO_Port GPIOA
#define MOTOR_IN2_Pin GPIO_PIN_1
#define MOTOR_IN2_GPIO_Port GPIOA
#define MOTOR2_IN1_Pin GPIO_PIN_4
#define MOTOR2_IN1_GPIO_Port GPIOA
#define MOTOR2_IN2_Pin GPIO_PIN_0
#define MOTOR2_IN2_GPIO_Port GPIOB
#define STEP_IN2_Pin GPIO_PIN_1
#define STEP_IN2_GPIO_Port GPIOB
#define STEP_IN1_Pin GPIO_PIN_2
#define STEP_IN1_GPIO_Port GPIOB
#define IR_SENSOR2_Pin GPIO_PIN_10
#define IR_SENSOR2_GPIO_Port GPIOB
#define STEP_IN4_Pin GPIO_PIN_14
#define STEP_IN4_GPIO_Port GPIOB
#define STEP_IN3_Pin GPIO_PIN_15
#define STEP_IN3_GPIO_Port GPIOB
#define IR_SENSOR6_Pin GPIO_PIN_7
#define IR_SENSOR6_GPIO_Port GPIOC
#define IR_SENSOR1_Pin GPIO_PIN_8
#define IR_SENSOR1_GPIO_Port GPIOA
#define IR_SENSOR7_Pin GPIO_PIN_9
#define IR_SENSOR7_GPIO_Port GPIOA
#define STEP_ENA_Pin GPIO_PIN_11
#define STEP_ENA_GPIO_Port GPIOA
#define STEP_ENB_Pin GPIO_PIN_12
#define STEP_ENB_GPIO_Port GPIOA
#define IR_SENSOR3_Pin GPIO_PIN_4
#define IR_SENSOR3_GPIO_Port GPIOB
#define IR_SENSOR4_Pin GPIO_PIN_5
#define IR_SENSOR4_GPIO_Port GPIOB
#define IR_SENSOR5_Pin GPIO_PIN_6
#define IR_SENSOR5_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
