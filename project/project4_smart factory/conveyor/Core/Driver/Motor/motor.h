#ifndef MOTOR_H
#define MOTOR_H

#include "common.h"

/* 모터 채널 ID */
typedef enum {
    MOTOR_CH1 = 0,   /* L298N #1 - Motor A : PA0,  PA1,  TIM3_CH1 */
    MOTOR_CH2,       /* L298N #1 - Motor B : PA4,  PB0,  TIM3_CH2 */
    MOTOR_CH3,       /* L298N #2 - Motor A : PC0,  PC1,  TIM4_CH3 */
    MOTOR_CH4        /* L298N #2 - Motor B : PC2,  PC3,  TIM4_CH4 */
} MotorChannel_t;

void Motor_Init(void);
void Motor_Forward(MotorChannel_t ch, uint16_t duty);
void Motor_Stop(MotorChannel_t ch);
void Motor_StopAll(void);

#endif /* MOTOR_H */
