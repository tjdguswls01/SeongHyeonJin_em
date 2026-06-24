

#include "main.h"
#include "button.h"
#include "i2c_lcd.h"
#include "cmsis_os.h"
#include "ultrasonic.h"

extern TIM_HandleTypeDef htim3;
extern volatile uint8_t bt_data;
extern int distance_center, distance_left, distance_right;

void mode_check(void);
void drive_car_main(void);
void auto_drive(void);
void manual_mode_run(void);

/* ===== AUTO MODE STATE ===== */
uint8_t auto_mode_state = 0;

/*
 * 1. LEFT MOTOR
 *    PC6 : IN1
 *    PC7 : IN2
 * 2. RIGHT MOTOR
 *    PC8 : IN3
 *    PC9 : IN4
 */
void left_forward(void)
{
	HAL_GPIO_WritePin(IN1_GPIO_Port, IN1_Pin, 1);
	HAL_GPIO_WritePin(IN2_GPIO_Port, IN2_Pin, 0);
}

void right_forward(void)
{
	HAL_GPIO_WritePin(IN3_GPIO_Port, IN3_Pin, 1);
	HAL_GPIO_WritePin(IN4_GPIO_Port, IN4_Pin, 0);
}

void left_backward(void)
{
	HAL_GPIO_WritePin(IN1_GPIO_Port, IN1_Pin, 0);
	HAL_GPIO_WritePin(IN2_GPIO_Port, IN2_Pin, 1);
}

void right_backward(void)
{
	HAL_GPIO_WritePin(IN3_GPIO_Port, IN3_Pin, 0);
	HAL_GPIO_WritePin(IN4_GPIO_Port, IN4_Pin, 1);
}

void all_foward(void)
{
	left_forward();
	right_forward();
}

void all_backward(void)
{
	left_backward();
	right_backward();
}

void left_speed(uint16_t speed)
{
	if (speed > 100) speed = 100;
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, speed);
}

void right_speed(uint16_t speed)
{
	if (speed > 100) speed = 100;
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, speed);
}

void all_stop(void)
{
	left_speed(0);
	right_speed(0);
}

void forward(int speed)
{
	all_foward();
	left_speed(speed);
	right_speed(speed);
}

void backward(int speed)
{
	all_backward();
	left_speed(speed);
	right_speed(speed);
}

void stop(void)
{
	all_stop();
	HAL_GPIO_WritePin(IN1_GPIO_Port, IN1_Pin, 1);
	HAL_GPIO_WritePin(IN2_GPIO_Port, IN2_Pin, 1);
	HAL_GPIO_WritePin(IN3_GPIO_Port, IN3_Pin, 1);
	HAL_GPIO_WritePin(IN4_GPIO_Port, IN4_Pin, 1);
}

/* ===== AUTO MODE ONLY TURN ===== */
void turn_left_only_back(int speed)
{
	all_stop();
	left_backward();
	left_speed(speed);
	right_speed(0);
}

void turn_right_only_back(int speed)
{
	all_stop();
	right_backward();
	right_speed(speed);
	left_speed(0);
}

/* ===== MAIN DRIVE LOOP ===== */
void drive_car_main()
{
	while (1)
	{
		manual_mode_run();
		mode_check();

		if (auto_mode_state)
		{
			auto_drive();
		}
	}
}

/* ===== MODE CHANGE BY BUTTON ===== */
void mode_check()
{
	if (get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, BUTTON0) == BUTTON_PRESS)
	{
		auto_mode_state = !auto_mode_state;

		move_cursor(0,0);
		if (auto_mode_state)
		{
			lcd_string("AUTO Mode       ");
		}
		else
		{
			lcd_string("Manual Mode    ");
			stop();
		}
		move_cursor(1,0);
		lcd_string("                ");
	}
}

void auto_drive(void)
{
	if (!auto_mode_state) return;

	int limit = 15;
	int fast_speed = 100;
	int back_speed = 60;     // 후진 속도 조금 증가
	int turn_speed = 70;     // 회전 시 속도 증가 → 각도 증가

	// 센서 신뢰 보정: 0이면 999 처리
	int c = (distance_center > 0) ? distance_center : 999;
	int l = (distance_left   > 0) ? distance_left   : 999;
	int r = (distance_right  > 0) ? distance_right  : 999;

	/* ===== 정면 장애물 ===== */
	if (c <= limit)
	{
		// 후진 시간 증가로 꺾임 각도 확대
		backward(back_speed);
		osDelay(400);

		if (l > r)
			turn_left_only_back(turn_speed);
		else
			turn_right_only_back(turn_speed);

		osDelay(450);  // 회전 유지 시간 증가
	}

	/* ===== 우측 근접 장애물 ===== */
	else if (r <= limit)
	{
		turn_left_only_back(turn_speed);
		osDelay(400);  // 회전 유지 시간 증가
	}

	/* ===== 좌측 근접 장애물 ===== */
	else if (l <= limit)
	{
		turn_right_only_back(turn_speed);
		osDelay(400);  // 회전 유지 시간 증가
	}

	/* ===== 고속 직진 ===== */
	else
	{
		forward(fast_speed);
	}
}



#if 0
/* ===== AUTO DRIVE LOGIC ===== */
void auto_drive(void)
{
	if (!auto_mode_state) return;

	int base_speed = 60;
	int turn_speed = 60;
	int limit = 15;

	if (distance_center > 0 && distance_center <= limit)
	{
		stop(); osDelay(200);
		backward(base_speed); osDelay(600);
		stop(); osDelay(200);

		int l = (distance_left  <= 0) ? 999 : distance_left;
		int r = (distance_right <= 0) ? 999 : distance_right;

		if (l > r) turn_left_only_back(turn_speed);
		else       turn_right_only_back(turn_speed);

		osDelay(500);
		stop(); osDelay(200);
	}
	else if (distance_right > 0 && distance_right <= limit)
	{
		turn_left_only_back(turn_speed);
	}
	else if (distance_left > 0 && distance_left <= limit)
	{
		turn_right_only_back(turn_speed);
	}
	else
	{
		forward(base_speed);
	}
}
#endif
/* ===== MANUAL MODE (BLUETOOTH) ===== */
void manual_mode_run(void)
{
	if (auto_mode_state) return;

	switch (bt_data)
	{
		case 'F': forward(100); break;
		case 'B': backward(100); break;
		case 'L': turn_left_only_back(100); break;
		case 'R': turn_right_only_back(100); break;
		case 'S': stop(); break;
		default: break;
	}
}



#if 0

#include "main.h"
#include "button.h"
#include "i2c_lcd.h"
#include "cmsis_os.h"

extern TIM_HandleTypeDef htim3;

void mode_check();
void drive_car_main();
void auto_drive(void);
void left_forward(void);
void right_forward(void);
void all_forward(void);
void manual_mode_run(void);

uint8_t auto_mode_state=0;

/*
 * 1. LEFT MOTOR
 *    PC6 : IN1
 *    PC7 : IN2
 * 2. RIGHT MOTOR
 *    PC8 : IN3
 *    PC9 : IN4
 *
 *      IN1 IN3    IN2.IN4
 *    =========   =========
 *        0           0   : 역회전
 *        1           0   : 정회전
 *        1           1   : 정지
 *
 */
void left_forward(void)
{
	HAL_GPIO_WritePin(IN1_GPIO_Port, IN1_Pin, 1);
	HAL_GPIO_WritePin(IN2_GPIO_Port, IN2_Pin, 0);
}

void right_forward(void)
{
	HAL_GPIO_WritePin(IN3_GPIO_Port, IN3_Pin, 1);
	HAL_GPIO_WritePin(IN4_GPIO_Port, IN4_Pin, 0);
}

void all_forward(void)
{
	left_forward();
	right_forward();
}
void drive_car_main()
{
    while (1)
    {
		manual_mode_run();       // bluetooth car command  run
		mode_check();            // button1 check

		if (auto_mode_state)
		{
			auto_drive();
		}
    }

}

void mode_check()
{
	if (get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, BUTTON0) == BUTTON_PRESS)
	{
		auto_mode_state = !auto_mode_state;

		move_cursor(0,0);
		if (auto_mode_state)
		{
			lcd_string("AUTO Mode       ");
		}
		else
		{
			lcd_string("Manual Mode    ");
		}
		move_cursor(1,0);
		lcd_string("                ");
	}
}

// 자율주행 프로그램을 이곳에 programming 한다.
void auto_drive(void)
{

}

extern volatile uint8_t bt_data;   // 2. BT로 부터 1byte의 INT가 들어오면 저장 하는 변수

void manual_mode_run(void)
{
	move_cursor(1,0);
	if (bt_data == 'F')
	{
		lcd_string("Forward    ");
	}
	else if (bt_data == 'B')
	{
		lcd_string("Backward    ");
	}
	else if (bt_data == 'L')
	{
		lcd_string("Turn Left    ");
	}
	else if (bt_data == 'R')
	{
		lcd_string("Turn Right   ");
	}


	switch(bt_data)
	{
		case 'F':
		forward(100);
		break;
		case 'B':
		backward(100);
		break;
		case 'L':
		turn_left(100);
		break;
		case 'R':
		turn_right(100);
		break;
		case 'S':
		stop();
		break;
		default:
		break;
	}
}

void forward(int speed)
{
	all_foward();

	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, speed);   // left speed

	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, speed);   //  right speed
}

void backward(int speed)
{
	all_backward();

	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, speed);   // left speed

	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, speed);   //  right speed
}

void turn_left(int speed)
{
	all_foward();

	left_speed(speed);
	right_speed(0);   //  PWM 출력 right
}

void turn_right(int speed)
{
	all_foward();

	left_speed(0); //  PWM 출력	  left
	right_speed(speed);    //  PWM 출력 right
}

void stop()
{
	all_stop();

	HAL_GPIO_WritePin(IN1_GPIO_Port, IN1_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(IN2_GPIO_Port, IN2_Pin, GPIO_PIN_SET);

	HAL_GPIO_WritePin(IN3_GPIO_Port, IN3_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(IN4_GPIO_Port, IN4_Pin, GPIO_PIN_SET);

}


void left_speed(uint16_t speed)
{
	if (speed >= 100) speed = 100;
	else if (speed < 0) speed = 0;

	if (speed == 0)
	{
		HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
	}
	else
	{
		HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, speed);
	}

}

void right_speed(uint16_t speed)
{
	if (speed >= 100) speed = 100;
	else if (speed < 0) speed = 0;

	if (speed == 0)
	{
		HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_2);
	}
	else
	{
		HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, speed);
	}
}

void all_foward()
{
	 left_forward();
	 right_forward();
}

void left_backward()
{
	HAL_GPIO_WritePin(IN1_GPIO_Port, IN1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(IN2_GPIO_Port, IN2_Pin, GPIO_PIN_SET);

}

void right_backward()
{
	HAL_GPIO_WritePin(IN3_GPIO_Port, IN3_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(IN4_GPIO_Port, IN4_Pin, GPIO_PIN_SET);
}

void all_backward()
{
	left_backward();
	right_backward();
}

void left_stop()
{
	left_speed(0);
}

void right_stop()
{
	right_speed(0);
}

void all_stop()
{
	 left_stop();
	 right_stop();
}


void dcmotor_main(void)
{
	static int PWM_val=0;
	static uint8_t direction=0;   // defult 0 : forward

	all_forward();

	while(1)
	{
		if (get_button(GPIOC, GPIO_PIN_1 , BUTTON1 )  == BUTTON_PRESS)
		{
			if (!direction)
			{
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, 1);
				PWM_val = __HAL_TIM_GET_COMPARE(&htim3, TIM_CHANNEL_1);
				PWM_val += 10;
				if (PWM_val > 100) {
					direction = !direction;
					PWM_val = 100;
				}
				__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, PWM_val);
				__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, PWM_val);

			} else {
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, 0);
				PWM_val = __HAL_TIM_GET_COMPARE(&htim3, TIM_CHANNEL_1);
				PWM_val -= 10;
				if (PWM_val < 0) {
					direction = !direction;
					PWM_val = 0;
				}
				__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, PWM_val);
				__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, PWM_val);
			}
		}
	}
}
#endif 
