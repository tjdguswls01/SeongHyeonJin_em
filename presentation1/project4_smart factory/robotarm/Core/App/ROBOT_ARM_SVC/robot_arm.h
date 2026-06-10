#ifndef CORE_APP_ROBOT_ARM_SVC_ROBOT_ARM_H_
#define CORE_APP_ROBOT_ARM_SVC_ROBOT_ARM_H_

#include "stm32f4xx_hal.h"
#include "../../Driver/uart_com/uart_com.h"

#define SERVO_COUNT     6
#define SEQUENCE_STEPS  8
#define TEST_MODE       0

#define SERVO_MIN_CCR   500
#define SERVO_MAX_CCR   2500
#define SERVO_MAX_ANGLE 360.0f

void RobotArm_Init(TIM_HandleTypeDef *htim2, TIM_HandleTypeDef *htim3,
                   TIM_HandleTypeDef *htim4);
void RobotArm_SetAngles(float *angles);
void RobotArm_SetStopFlag(uint8_t val);   // ★ 추가
void RobotArm_Run(void);

#endif
