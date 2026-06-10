#ifndef DRIVER_UART_COM_UART_COM_H_
#define DRIVER_UART_COM_UART_COM_H_

#include "stm32f4xx_hal.h"

typedef enum {
    ROBOT_STATUS_IDLE = 0,
    ROBOT_STATUS_MOVING,
    ROBOT_STATUS_DONE,
    ROBOT_STATUS_ERROR,
} RobotStatus_t;

typedef enum {
    CMD_NONE = 0,
    CMD_STOP,
    CMD_RESUME,
    CMD_ANGLES
} UartCommand_t;

void UART_Com_Init(UART_HandleTypeDef *huart);
void UART_Com_SendStatus(RobotStatus_t status);
void UART_Com_SendDone(void);
void UART_Com_SendStopped(void);
void UART_Com_ReceiveAngles(float *angles);
UartCommand_t UART_Com_ReadLine(uint8_t *buf, uint8_t buf_len);

#endif
