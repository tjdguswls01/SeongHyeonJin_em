#ifndef IR_SENSOR_H
#define IR_SENSOR_H

#include "common.h"

/* DC모터용 IR 채널 (IR_SENSOR1~4) */
typedef enum {
    IR_CH1 = 0,   /* PA8  → MOTOR_CH1 */
    IR_CH2,       /* PB10 → MOTOR_CH2 */
    IR_CH3,       /* PB4  → MOTOR_CH3 */
    IR_CH4        /* PB5  → MOTOR_CH4 */
} IR_Channel_t;

/* 스텝모터용 IR 채널 (IR_SENSOR5~7) */
typedef enum {
    STEP_IR_CH1 = 0,   /* PB6  */
    STEP_IR_CH2,       /* PC7  */
    STEP_IR_CH3        /* PA9  */
} StepIR_Channel_t;

/**
 * @brief  DC모터용 IR 센서 감지 여부
 * @param  ch    IR_CH1 ~ IR_CH4
 * @retval 1: 감지, 0: 미감지
 */
uint8_t IR_Detected(IR_Channel_t ch);

/**
 * @brief  스텝모터용 IR 센서 감지 여부
 * @param  ch    STEP_IR_CH1 ~ STEP_IR_CH3
 * @retval 1: 감지, 0: 미감지
 */
uint8_t StepIR_Detected(StepIR_Channel_t ch);

#endif /* IR_SENSOR_H */
