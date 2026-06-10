#ifndef APP_MAIN_H
#define APP_MAIN_H


typedef enum {
    CONV_NORMAL = 0,
    CONV_FORCE  = 1,
} ConvMode_t;

/**
 * @brief  애플리케이션 초기화 — 전체 PWM 시작, 전체 모터 정지
 */
void App_Init(void);

/**
 * @brief  애플리케이션 루프 — while(1) 내부에서 반복 호출
 */
void App_Run(void);

#endif /* APP_MAIN_H */
