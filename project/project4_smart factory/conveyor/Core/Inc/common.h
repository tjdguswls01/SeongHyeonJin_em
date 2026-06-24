#ifndef COMMON_H
#define COMMON_H

#include "main.h"
#include <stdint.h>

/* PWM 설정 */
#define MOTOR_PWM_MAX        999U

#define MOTOR1_DUTY  999U   /* 100% */
#define MOTOR2_DUTY  999U   /* 100% */
#define MOTOR3_DUTY  999U   /* 100% */
#define MOTOR4_DUTY  500U   /* 100% */

/* IR 센서 활성 레벨 */
#define SENSOR_ACTIVE_LEVEL  GPIO_PIN_SET

/* 컨베이어 채널 수 */
#define CONVEYOR_COUNT       4U

/* 스텝모터 IR 센서 수 */
#define STEP_SENSOR_COUNT    3U

/* 스텝모터 속도 프리셋 (us, 작을수록 빠름) */
#define STEP_SPEED_SLOW    8000U   /* 0.6 rev/sec */
#define STEP_SPEED_NORMAL  4000U   /* 1.0 rev/sec */
#define STEP_SPEED_FAST    3000U   /* 1.7 rev/sec */
#define STEP_SPEED_MAX     2000U   /* 2.5 rev/sec */
#define STEP_SPEED_MIN     1700U   /* 하한 제한 */

#endif /* COMMON_H */
