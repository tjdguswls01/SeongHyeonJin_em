#include "uart_com.h"
#include <string.h>
#include <stdio.h>

static UART_HandleTypeDef *_huart;

void UART_Com_Init(UART_HandleTypeDef *huart) {
    _huart = huart;
}

void UART_Com_SendStatus(RobotStatus_t status) {
    char buf[64];
    switch (status) {
        case ROBOT_STATUS_IDLE:
            snprintf(buf, sizeof(buf), "{\"robot1_status\":\"idle\"}\n");
            break;
        case ROBOT_STATUS_MOVING:
            snprintf(buf, sizeof(buf), "{\"robot1_status\":\"moving\"}\n");
            break;
        case ROBOT_STATUS_DONE:
            snprintf(buf, sizeof(buf), "{\"robot1_status\":\"done\"}\n");
            break;
        case ROBOT_STATUS_ERROR:
            snprintf(buf, sizeof(buf), "{\"robot1_status\":\"error\"}\n");
            break;
        default:
            return;
    }
    HAL_UART_Transmit(_huart, (uint8_t *)buf, strlen(buf), 100);
}

void UART_Com_SendDone(void) {
    HAL_UART_Transmit(_huart, (uint8_t *)"DONE\n", 5, 100);
}

void UART_Com_SendStopped(void) {
    HAL_UART_Transmit(_huart, (uint8_t *)"STOPPED\n", 8, 100);
}

// 한 줄 읽어서 명령어 판별 (논블로킹: 타임아웃 1ms)
UartCommand_t UART_Com_ReadLine(uint8_t *buf, uint8_t buf_len) {
    uint8_t idx = 0;
    uint8_t byte;

    while (idx < buf_len - 1) {
        if (HAL_UART_Receive(_huart, &byte, 1, 1) != HAL_OK) break;
        if (byte == '\n') break;
        buf[idx++] = byte;
    }
    buf[idx] = '\0';

    if (idx == 0)                               return CMD_NONE;
    if (strncmp((char*)buf, "STOP",   4) == 0)  return CMD_STOP;
    if (strncmp((char*)buf, "RESUME", 6) == 0)  return CMD_RESUME;
    return CMD_ANGLES;
}

// 기존 함수 유지 (호환성)
void UART_Com_ReceiveAngles(float *angles) {
    uint8_t buf[64] = {0};
    uint8_t idx = 0;
    uint8_t byte;

    while (1) {
        if (HAL_UART_Receive(_huart, &byte, 1, 10) == HAL_OK) {
            if (byte == '\n') break;
            buf[idx++] = byte;
            if (idx >= 63) break;
        } else {
            return;
        }
    }

    int a[6] = {0};
    int parsed = sscanf((char *)buf, "%d,%d,%d,%d,%d,%d",
                        &a[0], &a[1], &a[2], &a[3], &a[4], &a[5]);
    if (parsed == 6) {
        for (int i = 0; i < 6; i++) angles[i] = (float)a[i];
    }
}
