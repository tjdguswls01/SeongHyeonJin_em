#include "../Step_motor/step_motor.h"

extern TIM_HandleTypeDef htim2;

/* 4상 풀스텝 시퀀스 (IN1, IN2, IN3, IN4) */
static const uint8_t s_seq[4][4] = {
    {1, 0, 1, 0},
    {0, 1, 1, 0},
    {0, 1, 0, 1},
    {1, 0, 0, 1},
};

//idx=0 → {1,0,1,0} → IN1=H, IN2=L, IN3=H, IN4=L
//idx=1 → {0,1,1,0} → IN1=L, IN2=H, IN3=H, IN4=L
//idx=2 → {0,1,0,1} → IN1=L, IN2=H, IN3=L, IN4=H
//idx=3 → {1,0,0,1} → IN1=H, IN2=L, IN3=L, IN4=H

static uint8_t s_idx     = 0;
static uint8_t s_running = 0;

static void apply_step(uint8_t idx)
{
    /* STEP_IN1: PB2, STEP_IN2: PB1, STEP_IN3: PB15, STEP_IN4: PB14 */
    HAL_GPIO_WritePin(STEP_IN1_GPIO_Port, STEP_IN1_Pin,
                      s_seq[idx][0] ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(STEP_IN2_GPIO_Port, STEP_IN2_Pin,
                      s_seq[idx][1] ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(STEP_IN3_GPIO_Port, STEP_IN3_Pin,
                      s_seq[idx][2] ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(STEP_IN4_GPIO_Port, STEP_IN4_Pin,
                      s_seq[idx][3] ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void StepMotor_Init(void)
{
    /* ENA(PB3), ENB(PA10) HIGH 고정 → 최대 토크 */
    HAL_GPIO_WritePin(STEP_ENA_GPIO_Port, STEP_ENA_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(STEP_ENB_GPIO_Port, STEP_ENB_Pin, GPIO_PIN_SET);

    /* TIM2: 스텝 인터럽트 시작 (초기 5ms 간격) */
    HAL_TIM_Base_Start_IT(&htim2);
    s_running = 1;
}

void StepMotor_Stop(void)
{
    s_running = 0;
    HAL_GPIO_WritePin(STEP_IN1_GPIO_Port, STEP_IN1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(STEP_IN2_GPIO_Port, STEP_IN2_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(STEP_IN3_GPIO_Port, STEP_IN3_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(STEP_IN4_GPIO_Port, STEP_IN4_Pin, GPIO_PIN_RESET);
}

void StepMotor_SetSpeed(uint32_t step_interval_us)
{
    s_running = 1;
    if (step_interval_us < STEP_SPEED_MIN) step_interval_us = STEP_SPEED_MIN;
    __HAL_TIM_SET_AUTORELOAD(&htim2, step_interval_us - 1); // ARR 변경 Auto Reload Register
    __HAL_TIM_SET_COUNTER(&htim2, 0);
}

void StepMotor_ISR(void)// Interrupt Service Routine
{
    if (!s_running) return;
    apply_step(s_idx);
    s_idx = (s_idx + 1) % 4;
}
