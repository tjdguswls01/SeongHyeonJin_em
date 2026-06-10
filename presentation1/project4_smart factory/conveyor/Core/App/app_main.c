#include "app_main.h"
#include "../Driver/Motor/motor.h"
#include "../Driver/Step_motor/step_motor.h"
#include "../Driver/ir_sensor/ir_sensor.h"
#include "../Driver/uart_com/uart_com.h"
#include <stdio.h>

#define APP_LOOP_DELAY_MS  100U

static char s_msg[128];

/* DC모터 채널별 IR - Motor 매핑 테이블 */
static const struct {
    IR_Channel_t    ir;
    MotorChannel_t  motor;
    uint16_t        duty;
} s_conveyor[CONVEYOR_COUNT] = {
    { IR_CH1, MOTOR_CH1, MOTOR1_DUTY },
    { IR_CH2, MOTOR_CH2, MOTOR2_DUTY },
    { IR_CH3, MOTOR_CH3, MOTOR3_DUTY },
    { IR_CH4, MOTOR_CH4, MOTOR4_DUTY },
};

void App_Init(void)
{
    /* DC모터 초기화 */
    Motor_Init();

    /* 스텝모터 초기화 — 시작 시 바로 동작 */
    StepMotor_Init();
    StepMotor_SetSpeed(STEP_SPEED_MAX);

    UART_Print("System Init OK\r\n");
    UART_Print("=========================\r\n");
}

void App_Run(void)
{
    /* ── DC모터 IR 센서 상태 출력 ── */
    uint8_t dc1 = IR_Detected(IR_CH1);
    uint8_t dc2 = IR_Detected(IR_CH2);
    uint8_t dc3 = IR_Detected(IR_CH3);
    uint8_t dc4 = IR_Detected(IR_CH4);

    snprintf(s_msg, sizeof(s_msg),
             "[DC]   IR1=%d IR2=%d IR3=%d IR4=%d\r\n",
             dc1, dc2, dc3, dc4);
    UART_Print(s_msg);

    /* DC모터 제어 */
    for (uint8_t i = 0; i < CONVEYOR_COUNT; i++)
    {
    	if (IR_Detected(s_conveyor[i].ir))
    	    Motor_Forward(s_conveyor[i].motor, s_conveyor[i].duty);
    	else
    	    Motor_Stop(s_conveyor[i].motor);
    }

    /* ── 스텝모터 IR 센서 상태 출력 ── */
    uint8_t s1 = StepIR_Detected(STEP_IR_CH1);
    uint8_t s2 = StepIR_Detected(STEP_IR_CH2);
    uint8_t s3 = StepIR_Detected(STEP_IR_CH3);

    snprintf(s_msg, sizeof(s_msg),
             "[STEP] IR5=%d IR6=%d IR7=%d\r\n",
             s1, s2, s3);
    UART_Print(s_msg);

    /* 스텝모터 제어 */
//    if (s1 && s2 && s3)
        if (s2 && s3)
        StepMotor_SetSpeed(STEP_SPEED_NORMAL);
    else
        StepMotor_Stop();

    UART_Print("-------------------------\r\n");
    HAL_Delay(APP_LOOP_DELAY_MS);
}


///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
//
//#include "app_main.h"
//#include "../Driver/Motor/motor.h"
//#include "../Driver/Step_motor/step_motor.h"
//#include "../Driver/ir_sensor/ir_sensor.h"
//#include "../Driver/uart_com/uart_com.h"
//#include "main.h"
//#include <stdio.h>
//
//#define APP_LOOP_DELAY_MS  100U
//
//static char s_msg[128];
//
//typedef enum {
//    CONV_NORMAL = 0,
//    CONV_FORCE  = 1,
//} ConvMode_t;
//
//static ConvMode_t conv_mode = CONV_NORMAL;
//static uint8_t arm_done1_prev = 0;
//static uint8_t arm_done2_prev = 0;
//
///* DC모터 채널별 IR - Motor 매핑 테이블 */
//static const struct {
//    IR_Channel_t    ir;
//    MotorChannel_t  motor;
//    uint16_t        duty;
//} s_conveyor[CONVEYOR_COUNT] = {
//    { IR_CH1, MOTOR_CH1, MOTOR1_DUTY },
//    { IR_CH2, MOTOR_CH2, MOTOR2_DUTY },
//    { IR_CH3, MOTOR_CH3, MOTOR3_DUTY },
//    { IR_CH4, MOTOR_CH4, MOTOR4_DUTY },
//};
//
//void App_Init(void)
//{
//    Motor_Init();
//    StepMotor_Init();
//    StepMotor_SetSpeed(STEP_SPEED_MAX);
//
//    conv_mode = CONV_NORMAL;
//    arm_done1_prev = 0;
//    arm_done2_prev = 0;
//
//    UART_Print("System Init OK\r\n");
//    UART_Print("=========================\r\n");
//}
//
//void App_Run(void)
//{
//    /* ── DC모터 IR 센서 상태 출력 ── */
//    uint8_t dc1 = IR_Detected(IR_CH1);
//    uint8_t dc2 = IR_Detected(IR_CH2);
//    uint8_t dc3 = IR_Detected(IR_CH3);
//    uint8_t dc4 = IR_Detected(IR_CH4);
//
//    snprintf(s_msg, sizeof(s_msg),
//             "[DC]   IR1=%d IR2=%d IR3=%d IR4=%d\r\n",
//             dc1, dc2, dc3, dc4);
//    UART_Print(s_msg);
//
//    /* DC모터 제어 */
//    for (uint8_t i = 0; i < CONVEYOR_COUNT; i++) {
//        if (IR_Detected(s_conveyor[i].ir))
//            Motor_Forward(s_conveyor[i].motor, s_conveyor[i].duty);
//        else
//            Motor_Stop(s_conveyor[i].motor);
//    }
//
//    /* ── 스텝모터 IR 센서 상태 출력 ── */
//    uint8_t s2 = StepIR_Detected(STEP_IR_CH2);  // IR6: PC7
//    uint8_t s3 = StepIR_Detected(STEP_IR_CH3);  // IR7: PA9
//
//    snprintf(s_msg, sizeof(s_msg),
//             "[STEP] IR6=%d IR7=%d\r\n", s2, s3);
//    UART_Print(s_msg);
//
//    /* ── 로봇팔 완료 펄스 감지 ── */
//    uint8_t arm_done1_now = (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_4) == GPIO_PIN_SET) ? 1 : 0;
//    uint8_t arm_done2_now = (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_5) == GPIO_PIN_SET) ? 1 : 0;
//
//    if (arm_done1_prev == 0 && arm_done1_now == 1) {
//        conv_mode = CONV_FORCE;
//        UART_Print("[ARM1] Done → Force Move\r\n");
//    }
//    if (arm_done2_prev == 0 && arm_done2_now == 1) {
//        conv_mode = CONV_FORCE;
//        UART_Print("[ARM2] Done → Force Move\r\n");
//    }
//
//    arm_done1_prev = arm_done1_now;
//    arm_done2_prev = arm_done2_now;
//
//    /* ── 스텝모터 제어 ── */
//    if (conv_mode == CONV_FORCE) {
//        // 강제 이동: IR6 무시
//        StepMotor_SetSpeed(STEP_SPEED_NORMAL);
//        if (s3) {  // IR7 감지되면 정상 복귀
//            conv_mode = CONV_NORMAL;
//            UART_Print("[CONV] Normal mode\r\n");
//        }
//    } else {
//        // 정상 모드: IR6, IR7 둘 다 감지될 때만 이동
//        if (s2 && s3)
//            StepMotor_SetSpeed(STEP_SPEED_NORMAL);
//        else
//            StepMotor_Stop();
//    }
//
//    UART_Print("-------------------------\r\n");
//    HAL_Delay(APP_LOOP_DELAY_MS);
//}
