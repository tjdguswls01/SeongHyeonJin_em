#include "../Motor/motor.h"

extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;

typedef struct {
    GPIO_TypeDef      *in1_port;
    uint16_t           in1_pin;
    GPIO_TypeDef      *in2_port;
    uint16_t           in2_pin;
    TIM_HandleTypeDef *htim;
    uint32_t           tim_ch;
} MotorHW_t;

static const MotorHW_t s_motor[CONVEYOR_COUNT] = {
    /* CH1: L298N #1 Motor A — PA0, PA1, TIM3_CH1 */
    { MOTOR_IN1_GPIO_Port,  MOTOR_IN1_Pin,
      MOTOR_IN2_GPIO_Port,  MOTOR_IN2_Pin,  &htim3, TIM_CHANNEL_1 },
    /* CH2: L298N #1 Motor B — PA4, PB0, TIM3_CH2 */
    { MOTOR2_IN1_GPIO_Port, MOTOR2_IN1_Pin,
      MOTOR2_IN2_GPIO_Port, MOTOR2_IN2_Pin, &htim3, TIM_CHANNEL_2 },
    /* CH3: L298N #2 Motor A — PC0, PC1, TIM4_CH3 */
    { MOTOR3_IN1_GPIO_Port, MOTOR3_IN1_Pin,
      MOTOR3_IN2_GPIO_Port, MOTOR3_IN2_Pin, &htim4, TIM_CHANNEL_3 },
    /* CH4: L298N #2 Motor B — PC2, PC3, TIM4_CH4 */
    { MOTOR4_IN1_GPIO_Port, MOTOR4_IN1_Pin,
      MOTOR4_IN2_GPIO_Port, MOTOR4_IN2_Pin, &htim4, TIM_CHANNEL_4 },
};

void Motor_Init(void)
{
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
    Motor_StopAll();
}

void Motor_Forward(MotorChannel_t ch, uint16_t duty)
{
    if (ch >= CONVEYOR_COUNT) return;
    if (duty > MOTOR_PWM_MAX) duty = MOTOR_PWM_MAX;

    const MotorHW_t *m = &s_motor[ch];
    HAL_GPIO_WritePin(m->in1_port, m->in1_pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(m->in2_port, m->in2_pin, GPIO_PIN_RESET);
    __HAL_TIM_SET_COMPARE(m->htim, m->tim_ch, duty);
}

void Motor_Stop(MotorChannel_t ch)
{
    if (ch >= CONVEYOR_COUNT) return;

    const MotorHW_t *m = &s_motor[ch];
    HAL_GPIO_WritePin(m->in1_port, m->in1_pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(m->in2_port, m->in2_pin, GPIO_PIN_RESET);
    __HAL_TIM_SET_COMPARE(m->htim, m->tim_ch, 0);
}

void Motor_StopAll(void)
{
    for (uint8_t i = 0; i < CONVEYOR_COUNT; i++)
    {
        Motor_Stop((MotorChannel_t)i);
    }
}
