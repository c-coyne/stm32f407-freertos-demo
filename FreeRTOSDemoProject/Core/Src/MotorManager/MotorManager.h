
/*=====================================================================================*\
| Author:   Christopher Coyne                                           July 1st, 2024  |
| --------------------------------------------------------------------------------------|
| Date:     July 1st, 2024                                                              |
| --------------------------------------------------------------------------------------|
| MODULE:     [ MotorManager ]                                                          |
| FILE:       MotorManager.h                                                            |
| --------------------------------------------------------------------------------------|
| DESCRIPTION:                                                                          |
|    The `MotorManager` module is responsible for handling user input and speed         |
|    control for the motor.                                                             |
\*=====================================================================================*/

#ifndef MOTORMANAGER_H_
#define MOTORMANAGER_H_

/****************************************************
 *  Include files                                   *
 ****************************************************/

#include "FreeRTOS.h"
#include "main.h"

/****************************************************
 *  Public functions                                *
 ****************************************************/

void motor_task(void *param);
void motor_gpio_callback(uint16_t GPIO_Pin);
void motor_timer_callback(TIM_HandleTypeDef *htim);
void motor_report_callback(void);

/****************************************************
 *  Variables                                       *
 ****************************************************/

extern volatile int32_t encoder_count;
extern volatile float motor_speed;

// Typedefs
typedef enum {
	None = 0,
	PID
} motor_algo_t;

#endif /* MOTORMANAGER_H_ */
