/*
 * ir_sensor.h
 *
 *  Created on: Apr 13, 2026
 *      Author: kccistc
 */

#ifndef DRIVER_IR_SENSOR_IR_SENSOR_H_
#define DRIVER_IR_SENSOR_IR_SENSOR_H_

#include "stm32f4xx_hal.h"
#include "main.h"

// 적외선 센서 감지 플래그
extern volatile uint8_t ir_detected;

void IR_Sensor_Init(void);
uint8_t IR_Sensor_IsDetected(void);
void IR_Sensor_ClearFlag(void);


#endif /* DRIVER_IR_SENSOR_IR_SENSOR_H_ */
