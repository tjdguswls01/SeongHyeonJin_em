#ifndef UART_COM_H
#define UART_COM_H

#include "common.h"

/**
 * @brief  문자열을 UART2로 전송
 * @param  msg  null-terminated 문자열
 */
void UART_Print(const char *msg);

#endif /* UART_COM_H */
