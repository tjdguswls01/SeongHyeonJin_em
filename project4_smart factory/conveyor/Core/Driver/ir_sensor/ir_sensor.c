#include "../ir_sensor/ir_sensor.h"

typedef struct {
    GPIO_TypeDef *port;
    uint16_t      pin;
} IR_HW_t;

/* DC모터용 IR 센서 (IR_SENSOR1~4) */
static const IR_HW_t s_ir[CONVEYOR_COUNT] = {
    { IR_SENSOR1_GPIO_Port, IR_SENSOR1_Pin },  /* IR_CH1: PA8  */
    { IR_SENSOR2_GPIO_Port, IR_SENSOR2_Pin },  /* IR_CH2: PB10 */
    { IR_SENSOR3_GPIO_Port, IR_SENSOR3_Pin },  /* IR_CH3: PB4  */
    { IR_SENSOR4_GPIO_Port, IR_SENSOR4_Pin },  /* IR_CH4: PB5  */
};

/* 스텝모터용 IR 센서 (IR_SENSOR5~7) */
static const IR_HW_t s_step_ir[STEP_SENSOR_COUNT] = {
    { IR_SENSOR5_GPIO_Port, IR_SENSOR5_Pin },  /* STEP_IR_CH1: PB6 */
    { IR_SENSOR6_GPIO_Port, IR_SENSOR6_Pin },  /* STEP_IR_CH2: PC7 */
    { IR_SENSOR7_GPIO_Port, IR_SENSOR7_Pin },  /* STEP_IR_CH3: PA9 */
};

uint8_t IR_Detected(IR_Channel_t ch)
{
    if (ch >= CONVEYOR_COUNT) return 0;
    return (HAL_GPIO_ReadPin(s_ir[ch].port, s_ir[ch].pin) == SENSOR_ACTIVE_LEVEL)
           ? 1U : 0U;
}

uint8_t StepIR_Detected(StepIR_Channel_t ch)
{
    if (ch >= STEP_SENSOR_COUNT) return 0;
    return (HAL_GPIO_ReadPin(s_step_ir[ch].port, s_step_ir[ch].pin) == SENSOR_ACTIVE_LEVEL)
           ? 1U : 0U;
}
