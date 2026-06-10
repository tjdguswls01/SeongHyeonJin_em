#include "robot_arm.h"
#include "../../Driver/servo/servo.h"
#include "../../Driver/gpio_signal/gpio_signal.h"
#include "../../Driver/uart_com/uart_com.h"

static Servo_t servo[SERVO_COUNT];
static volatile uint8_t g_stop_flag = 0;  // ★ 추가

void RobotArm_SetStopFlag(uint8_t val) {   // ★ 추가
    g_stop_flag = val;
}

static const float sequence[SEQUENCE_STEPS][SERVO_COUNT] = {
    {45,  45,  45,  45,  45,  45},
    {45,  20,  20,  45,  45,  45},
    {45,  20,  20,  45,  45,  20},
    {45,  45,  45,  45,  45,  20},
    {0,   45,  45,  45,  45,  20},
    {0,   20,  20,  45,  45,  20},
    {0,   20,  20,  45,  45,  45},
    {45,  45,  45,  45,  45,  45},
};

static const uint32_t step_delay[SEQUENCE_STEPS] = {
    500, 800, 600, 800, 1000, 800, 600, 800
};

static void ExecuteStep(uint8_t step) {
    for (int i = 0; i < SERVO_COUNT; i++) {
        Servo_SetAngleSmooth(&servo[i], sequence[step][i], 6);
    }
    HAL_Delay(step_delay[step]);
}

void RobotArm_Init(TIM_HandleTypeDef *htim2, TIM_HandleTypeDef *htim3,
                   TIM_HandleTypeDef *htim4) {
    Servo_Init(&servo[0], htim2, TIM_CHANNEL_1);
    Servo_Init(&servo[1], htim2, TIM_CHANNEL_2);
    Servo_Init(&servo[2], htim3, TIM_CHANNEL_1);
    Servo_Init(&servo[3], htim3, TIM_CHANNEL_2);
    Servo_Init(&servo[4], htim4, TIM_CHANNEL_1);
    Servo_Init(&servo[5], htim4, TIM_CHANNEL_2);

#if TEST_MODE
    int test_list[] = TEST_SERVOS;
    for (int i = 0; i < TEST_SERVO_COUNT; i++) {
        Servo_SoftStart(&servo[test_list[i]], 90.0f);
        HAL_Delay(200);
    }
    HAL_Delay(1000);
    while(1) {
        for (float a = 90.0f; a >= 70.0f; a -= 0.5f) {
            for (int i = 0; i < TEST_SERVO_COUNT; i++)
                Servo_SetAngle(&servo[test_list[i]], a);
            HAL_Delay(20);
        }
        HAL_Delay(500);
        for (float a = 70.0f; a <= 90.0f; a += 0.5f) {
            for (int i = 0; i < TEST_SERVO_COUNT; i++)
                Servo_SetAngle(&servo[test_list[i]], a);
            HAL_Delay(20);
        }
        HAL_Delay(500);
        for (float a = 90.0f; a <= 110.0f; a += 0.5f) {
            for (int i = 0; i < TEST_SERVO_COUNT; i++)
                Servo_SetAngle(&servo[test_list[i]], a);
            HAL_Delay(20);
        }
        HAL_Delay(500);
        for (float a = 110.0f; a >= 90.0f; a -= 0.5f) {
            for (int i = 0; i < TEST_SERVO_COUNT; i++)
                Servo_SetAngle(&servo[test_list[i]], a);
            HAL_Delay(20);
        }
        HAL_Delay(500);
    }
#else
    float init_angles[SERVO_COUNT] = {143.0f, 180.0f, 159.0f, 171.0f, 163.0f, 176.0f};
    for (int i = 0; i < SERVO_COUNT; i++) {
        Servo_SoftStart(&servo[i], init_angles[i]);
        HAL_Delay(200);
    }
#endif
}

void RobotArm_SetAngles(float *angles) {
    uint32_t target_ccr[SERVO_COUNT];
    for (int i = 0; i < SERVO_COUNT; i++) {
        float a = angles[i];
        if (a > 360.0f) a = 360.0f;
        if (a < 0.0f)   a = 0.0f;
        target_ccr[i] = (uint32_t)(SERVO_MIN_CCR +
                        (a / SERVO_MAX_ANGLE) * (SERVO_MAX_CCR - SERVO_MIN_CCR));
    }

    uint32_t current_ccr[SERVO_COUNT];
    for (int i = 0; i < SERVO_COUNT; i++) {
        current_ccr[i] = __HAL_TIM_GET_COMPARE(servo[i].htim, servo[i].channel);
    }

    int done = 0;
    while (!done) {

        // ★ 루프 안에서 직접 UART 체크
        uint8_t buf[32] = {0};
        UartCommand_t cmd = UART_Com_ReadLine(buf, sizeof(buf));
        if (cmd == CMD_STOP) {
            g_stop_flag = 1;
            UART_Com_SendStopped();
        } else if (cmd == CMD_RESUME) {
            g_stop_flag = 0;
        }

        // 정지 중 → 현재 위치 유지하며 대기
        if (g_stop_flag) {
            HAL_Delay(50);
            continue;
        }

        done = 1;
        for (int i = 0; i < SERVO_COUNT; i++) {
            if (current_ccr[i] < target_ccr[i]) {
                current_ccr[i] += 5;
                if (current_ccr[i] > target_ccr[i]) current_ccr[i] = target_ccr[i];
                done = 0;
            } else if (current_ccr[i] > target_ccr[i]) {
                current_ccr[i] -= 5;
                if (current_ccr[i] < target_ccr[i]) current_ccr[i] = target_ccr[i];
                done = 0;
            }
            __HAL_TIM_SET_COMPARE(servo[i].htim, servo[i].channel, current_ccr[i]);
        }
        HAL_Delay(10);
    }

    UART_Com_SendDone();
}

void RobotArm_Run(void) {
    UART_Com_SendStatus(ROBOT_STATUS_MOVING);
    for (uint8_t step = 1; step < SEQUENCE_STEPS; step++) {
        ExecuteStep(step);
    }
    GPIO_Signal_SendComplete();
    UART_Com_SendStatus(ROBOT_STATUS_DONE);
    HAL_Delay(200);
    UART_Com_SendStatus(ROBOT_STATUS_IDLE);
}
