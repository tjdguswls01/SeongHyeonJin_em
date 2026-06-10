#ifndef STEP_MOTOR_H
#define STEP_MOTOR_H

#include "common.h"

/**
 * @brief  스텝모터 초기화 — ENA/ENB HIGH 고정, TIM2 인터럽트 시작
 */
void StepMotor_Init(void);

/**
 * @brief  스텝모터 정지 — IN1~IN4 모두 LOW
 */
void StepMotor_Stop(void);

/**
 * @brief  스텝모터 속도 설정
 * @param  step_interval_us  스텝 간격 (us), 작을수록 빠름
 *                           최소 STEP_SPEED_MIN (1700us)
 */
void StepMotor_SetSpeed(uint32_t step_interval_us);

/**
 * @brief  TIM2 인터럽트 콜백에서 호출 — 1스텝 진행
 */
void StepMotor_ISR(void);

#endif /* STEP_MOTOR_H */
