
/*=====================================================================================*\
| Author:   Christopher Coyne                                          June 19th, 2024  |
| --------------------------------------------------------------------------------------|
| Date:     June 19th, 2024                                                             |
| --------------------------------------------------------------------------------------|
| MODULE:     [ LedManager ]                                                            |
| FILE:       LedManager.h                                                              |
| --------------------------------------------------------------------------------------|
| DESCRIPTION:                                                                          |
|    The `LedManager` module is responsible for handling user input and all effects     |
|    pertaining to the on-board LEDs.                                                   |
\*=====================================================================================*/

#ifndef SRC_LEDMANAGER_LEDMANAGER_H_
#define SRC_LEDMANAGER_LEDMANAGER_H_

/****************************************************
 *  Include files                                   *
 ****************************************************/

#include "FreeRTOS.h"
#include "timers.h"

/****************************************************
 *  Public functions                                *
 ****************************************************/

void led_task(void *param);
void led_callback(TimerHandle_t xTimer);

/****************************************************
 *  Variables                                       *
 ****************************************************/

typedef enum
{
	effectNone = 0,
	effectE1,
	effectE2,
	effectE3,
	effectE4
} led_effect_t;

#endif /* SRC_LEDMANAGER_LEDMANAGER_H_ */
