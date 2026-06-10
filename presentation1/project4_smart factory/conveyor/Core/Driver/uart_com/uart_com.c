#include "../uart_com/uart_com.h"
#include <string.h>

extern UART_HandleTypeDef huart2;

void UART_Print(const char *msg)
{
    HAL_UART_Transmit(&huart2,
                      (uint8_t *)msg,
                      (uint16_t)strlen(msg),
                      HAL_MAX_DELAY);
}
