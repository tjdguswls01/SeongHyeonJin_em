/*
 * servo.h
 *
 *  Created on: Apr 13, 2026
 *      Author: kccistc
 */

#ifndef DRIVER_SERVO_SERVO_H_
#define DRIVER_SERVO_SERVO_H_

#include "stm32f4xx_hal.h"

#define SERVO_MIN_CCR       500
#define SERVO_MAX_CCR       2500
#define SERVO_MAX_ANGLE     360.0f  // 360 → 180
#define SERVO_S1_MAX        360.0f  // 360 → 180
#define SERVO_OTHER_MAX     360.0f

typedef struct {
    TIM_HandleTypeDef *htim;
    uint32_t channel;
} Servo_t;

void Servo_Init(Servo_t *servo, TIM_HandleTypeDef *htim, uint32_t channel);
void Servo_SetAngle(Servo_t *servo, float angle);
void Servo_SetAngleSmooth(Servo_t *servo, float target_angle, uint32_t step_delay_ms);
void Servo_SoftStart(Servo_t *servo, float target_angle);

#endif /* DRIVER_SERVO_SERVO_H_ */
