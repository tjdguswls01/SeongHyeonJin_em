#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include <stdio.h>

/* 외부 변수 */
extern uint8_t auto_mode_state;
extern volatile int distance_left;
extern volatile int distance_center;
extern volatile int distance_right;

/* 거리 기준 */
#define WARN_DIST   25
#define STOP_DIST   15

void oled_display_main(void)
{
    char buf[32];
    static uint8_t blink = 0;   // ★ 점멸 토글 변수

    blink ^= 1;  // 호출될 때마다 0 ↔ 1

    /* 기본 화면 */
    ssd1306_Fill(Black);

    /* 🔴 AVOID / STOP 상태에서만 점멸 반전 */
    if (auto_mode_state && distance_center < WARN_DIST)
    {
        if (blink)
        {
            ssd1306_InvertRectangle(
                0, 0,
                SSD1306_WIDTH - 1,
                SSD1306_HEIGHT - 1
            );
        }
    }

    /* 제목 */
    ssd1306_SetCursor(0, 0);
    ssd1306_WriteString("Smart Car", Font_11x18, White);

    /* 초음파 한 줄 */
    sprintf(buf, "L:%3d C:%3d R:%3d",
            distance_left,
            distance_center,
            distance_right);
    ssd1306_SetCursor(0, 22);
    ssd1306_WriteString(buf, Font_7x10, White);

    /* STATUS 크게 표시 */
    ssd1306_SetCursor(0, 36);

    if (!auto_mode_state)
    {
        ssd1306_WriteString("MANUAL", Font_16x24, White);
    }
    else if (distance_center < STOP_DIST)
    {
        ssd1306_WriteString("STOP", Font_16x24, White);
    }
    else if (distance_center < WARN_DIST)
    {
        ssd1306_WriteString("AVOID", Font_16x24, White);

        ssd1306_SetCursor(0, 58);
        if (distance_left > distance_right)
            ssd1306_WriteString("LEFT", Font_7x10, White);
        else
            ssd1306_WriteString("RIGHT", Font_7x10, White);
    }
    else
    {
        ssd1306_WriteString("GO", Font_16x24, White);
    }

    ssd1306_UpdateScreen();
}


#if 0
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include <stdio.h>

/* 외부 변수 참조 */
extern uint8_t auto_mode_state;
extern volatile int distance_left;
extern volatile int distance_center;
extern volatile int distance_right;

void oled_display_main(void)
{
    char buf[32];

    ssd1306_Fill(Black);

    // 제목
    ssd1306_SetCursor(0, 0);
    ssd1306_WriteString("Smart Car", Font_11x18, White);

    // 모드 표시
    ssd1306_SetCursor(0, 20);
    if (auto_mode_state)
        ssd1306_WriteString("MODE : AUTO", Font_7x10, White);
    else
        ssd1306_WriteString("MODE : MANUAL", Font_7x10, White);

    // 초음파 정보 한 줄 출력
    sprintf(buf, "L:%3d C:%3d R:%3d",
            distance_left,
            distance_center,
            distance_right);

    ssd1306_SetCursor(0, 36);
    ssd1306_WriteString(buf, Font_7x10, White);

    ssd1306_UpdateScreen();
}
#endif