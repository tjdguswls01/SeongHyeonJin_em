#include "ap_main.h"
#include "ROBOT_ARM_SVC/robot_arm.h"
#include "../Driver/ir_sensor/ir_sensor.h"
#include "../Driver/gpio_signal/gpio_signal.h"
#include "../Driver/uart_com/uart_com.h"
#include <string.h>
#include <stdio.h>

// huart2를 IR 트리거 시 UART 전송에 사용하기 위해 저장
static UART_HandleTypeDef *_huart_app;

void App_Init(TIM_HandleTypeDef *htim2, TIM_HandleTypeDef *htim3,
              TIM_HandleTypeDef *htim4, UART_HandleTypeDef *huart2) {
    _huart_app = huart2;   // ★ 저장

    IR_Sensor_Init();
    GPIO_Signal_Init();
    UART_Com_Init(huart2);
    RobotArm_Init(htim2, htim3, htim4);
    UART_Com_SendStatus(ROBOT_STATUS_IDLE);
}

void App_Run(void) {
    // === 1. UART 명령 처리 (기존 로직 그대로) ===
    uint8_t buf[64] = {0};
    UartCommand_t cmd = UART_Com_ReadLine(buf, sizeof(buf));

    if (cmd == CMD_STOP) {
        RobotArm_SetStopFlag(1);
        UART_Com_SendStopped();
    } else if (cmd == CMD_RESUME) {
        RobotArm_SetStopFlag(0);
    } else if (cmd == CMD_ANGLES) {
        float angles[6] = {-1,-1,-1,-1,-1,-1};
        int a[6] = {0};
        int parsed = sscanf((char*)buf, "%d,%d,%d,%d,%d,%d",
                            &a[0],&a[1],&a[2],&a[3],&a[4],&a[5]);
        if (parsed == 6) {
            for (int i = 0; i < 6; i++) angles[i] = (float)a[i];
            RobotArm_SetAngles(angles);
        }
    }

    // === 2. ★ IR 센서 트리거 시 CNN 값을 파이썬에 알림 ===
    if (IR_Sensor_IsDetected()) {
        IR_Sensor_ClearFlag();

        uint8_t cnn = GPIO_Signal_ReadCNN();   // 0~3

        // 파이썬에 "IR_TRIG:N\n" 형식으로 전송
        char msg[32];
        int len = snprintf(msg, sizeof(msg), "IR_TRIG:%d\n", cnn);
        HAL_UART_Transmit(_huart_app, (uint8_t*)msg, len, 100);

        // 디바운스: 센서가 HIGH로 돌아올 때까지 대기 + 여유
        while (HAL_GPIO_ReadPin(IR_SENSOR_GPIO_Port, IR_SENSOR_Pin) == GPIO_PIN_RESET) {
            HAL_Delay(10);
        }
        HAL_Delay(200);
    }
}
