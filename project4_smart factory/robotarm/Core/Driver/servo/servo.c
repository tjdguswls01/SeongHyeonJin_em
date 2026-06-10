/*
 * servo.c
 *
 *  Created on: Apr 13, 2026
 *      Author: kccistc
 */

#include "servo.h"

void Servo_Init(Servo_t *servo, TIM_HandleTypeDef *htim, uint32_t channel) {
    servo->htim    = htim;
    servo->channel = channel;

    // 중간값(180도 = CCR 1500)으로 시작
    // RETURN 포즈가 143~180도 범위라서 크게 안 튀어요
    __HAL_TIM_SET_COMPARE(htim, channel, 1500);

    HAL_TIM_PWM_Start(htim, channel);
}

void Servo_SetAngle(Servo_t *servo, float angle) {
    if (angle < 0.0f)            angle = 0.0f;
    if (angle > SERVO_MAX_ANGLE) angle = SERVO_MAX_ANGLE;

    uint32_t ccr = (uint32_t)(SERVO_MIN_CCR +
                   (angle / SERVO_MAX_ANGLE) * (SERVO_MAX_CCR - SERVO_MIN_CCR));
    __HAL_TIM_SET_COMPARE(servo->htim, servo->channel, ccr);
}

void Servo_SetAngleSmooth(Servo_t *servo, float target_angle, uint32_t step_delay_ms) {
    uint32_t current_ccr = __HAL_TIM_GET_COMPARE(servo->htim, servo->channel);
    uint32_t target_ccr  = (uint32_t)(SERVO_MIN_CCR +
                           (target_angle / SERVO_MAX_ANGLE) * (SERVO_MAX_CCR - SERVO_MIN_CCR));

    if (current_ccr < target_ccr) {
        for (uint32_t ccr = current_ccr; ccr <= target_ccr; ccr += 3) {
            __HAL_TIM_SET_COMPARE(servo->htim, servo->channel, ccr);
            HAL_Delay(step_delay_ms);
        }
    } else {
        for (uint32_t ccr = current_ccr; ccr >= target_ccr + 3; ccr -= 3) {
            __HAL_TIM_SET_COMPARE(servo->htim, servo->channel, ccr);
            HAL_Delay(step_delay_ms);
        }
        __HAL_TIM_SET_COMPARE(servo->htim, servo->channel, target_ccr);
    }
}

void Servo_SoftStart(Servo_t *servo, float target_angle) {
    uint32_t current_ccr = __HAL_TIM_GET_COMPARE(servo->htim, servo->channel); //현재 타이머 레지스터에서 CCR값을 읽음
    float current_angle  = (float)(current_ccr - SERVO_MIN_CCR) /
                           (float)(SERVO_MAX_CCR - SERVO_MIN_CCR) * SERVO_MAX_ANGLE; //CCR값을 각도로 역산
                            
    float diff = target_angle - current_angle; //(이동거리 계산) 현재 각도에서 목표 각도까지 얼마나 이동해야 하는지 계산.
    int steps  = (int)(diff < 0 ? -diff : diff) * 4; //이동거리에 비례해서 스텝 수를 결정.
                                                     // 즉, 각도 차이가 클수록 더 많은 스텝으로 나눠서 이동
    if (steps < 1) steps = 1;

    for (int i = 0; i <= steps; i++) { //전체 이동거리를 steps 개로 나눠서 천천히 이동.
        float angle = current_angle + diff * i / steps;
        uint32_t ccr = (uint32_t)(SERVO_MIN_CCR +
                       (angle / SERVO_MAX_ANGLE) * (SERVO_MAX_CCR - SERVO_MIN_CCR));
        __HAL_TIM_SET_COMPARE(servo->htim, servo->channel, ccr);
        HAL_Delay(5); 
        //각 스텝마다 각도를 CCR로 변환해서 레지스터에 쓰고 5ms 대기
    }
}
