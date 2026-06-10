
#include "ultrasonic.h"
#include "timer.h"
#include "main.h"
#include <stdio.h>

// main.c에 선언된 변수를 참조
extern int distance_center, distance_left, distance_right;
extern volatile int TIM10_1ms_ultrasonic;

uint8_t sensor_idx = 0; // 0:Center, 1:Left, 2:Right

// 각 채널별 독립 상태 관리 변수
static uint8_t is_first_ch1 = 0;
static uint8_t is_first_ch2 = 0;
static uint8_t is_first_ch4 = 0;

void make_trigger(GPIO_TypeDef* port, uint16_t pin)
{
    HAL_GPIO_WritePin(port, pin, 0);
    delay_us(2);
    HAL_GPIO_WritePin(port, pin, 1);
    delay_us(10);
    HAL_GPIO_WritePin(port, pin, 0);
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM1)
    {
        // 1. 왼쪽 센서 (TIM1 Channel 1)
        if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
        {
            if (is_first_ch1 == 0) {
                __HAL_TIM_SET_COUNTER(htim, 0);
                is_first_ch1 = 1;
            } else {
                is_first_ch1 = 0;
                distance_left = (int)(HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1) * 0.034 / 2);
            }
        }
        // 2. 중앙 센서 (TIM1 Channel 2)
        else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
        {
            if (is_first_ch2 == 0) {
                __HAL_TIM_SET_COUNTER(htim, 0);
                is_first_ch2 = 1;
            } else {
                is_first_ch2 = 0;
                distance_center = (int)(HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2) * 0.034 / 2);
            }
        }
        // 3. 오른쪽 센서 (TIM1 Channel 4)
        else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4)
        {
            if (is_first_ch4 == 0) {
                __HAL_TIM_SET_COUNTER(htim, 0);
                is_first_ch4 = 1;
            } else {
                is_first_ch4 = 0;
                distance_right = (int)(HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_4) * 0.034 / 2);
            }
        }
    }
}

void ultrasonic_main()
{

    // 60ms마다 센서를 순차적으로 트리거하여 간섭 방지
    if (TIM10_1ms_ultrasonic >= 40)
    {
        TIM10_1ms_ultrasonic = 0;

        sensor_idx = (sensor_idx + 1) % 3;

        if (sensor_idx == 0) {
            is_first_ch2 = 0; // 신호 꼬임 방지를 위한 초기화
            make_trigger(CENTER_TRIG_GPIO_Port, CENTER_TRIG_Pin);
        }
        else if (sensor_idx == 1) {
            is_first_ch1 = 0;
            make_trigger(LEFT_TRIG_GPIO_Port, LEFT_TRIG_Pin);
        }
        else if (sensor_idx == 2) {
            is_first_ch4 = 0;
            make_trigger(RIGHT_TRIG_GPIO_Port, RIGHT_TRIG_Pin);
        }
    }
//    printf("L : %d C : %d R : %d \n",distance_left, distance_center, distance_right);
}

//ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ

#if 0
#include "ultrasonic.h"

void ultrasonic_main();

extern volatile int TIM10_1ms_ultrasonic;
int distance;   // 거리 측정값을 저장하는 변수
int ic_echo_flag;   // 완전한 echo pulse detect
// move from Drivers/STM32F4xx_HAL_Driver/stm32f4xx_hal_tim.c to here
// 상승/하강 에지 둘다 이곳으로 진입
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	static uint8_t is_first_capture=0;  // 상승 하강 구분
	// 1 TIMER 몇번 인가 확인
	if (htim->Instance == TIM1)
	{
		// 2.상승에지 인가 하강에지 인가 체크

		if (is_first_capture == 0) //  2.1 상승에지인경우 : timer counter를 clear
		{
			__HAL_TIM_SET_COUNTER(htim, 0);  // counter clear
			is_first_capture=1;   // 상승에지 detect flag
		}
		else if (is_first_capture == 1)   // 하강
		{
		//  2.2 하강에지인경우 : 현재 까지의 counter값을 읽는 작업을 한다.
			is_first_capture=0;
			distance = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
			ic_echo_flag=1;
		}
	}

}

void make_trigger(void)
{
	HAL_GPIO_WritePin(CENTER_TRIG_GPIO_Port, CENTER_TRIG_Pin, 0);
	delay_us(2);
	HAL_GPIO_WritePin(CENTER_TRIG_GPIO_Port, CENTER_TRIG_Pin, 1);
	delay_us(10);
	HAL_GPIO_WritePin(CENTER_TRIG_GPIO_Port, CENTER_TRIG_Pin, 0);
}
void ultrasonic_main()
{
	if (TIM10_1ms_ultrasonic >= 1000)
	{
		TIM10_1ms_ultrasonic=0;
		// 1. make trigger
		make_trigger();
		// 2. 에코펄스가 왔으면 거리 계산 하여 출력 한다.
		if (ic_echo_flag)
		{
			ic_echo_flag=0;
			distance = distance * 0.034 / 2;  // 1us가 0.034cm
			// 왕복값이 넘어 오기 떄문에 2로 나눈다. 편도만 있으면 되니까
			printf("dis1: %d\n", distance);
		}
	}
}

#endif