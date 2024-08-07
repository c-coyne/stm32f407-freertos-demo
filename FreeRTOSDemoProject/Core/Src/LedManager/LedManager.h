
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

#ifndef LEDMANAGER_H_
#define LEDMANAGER_H_

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
	effectE1 = 0,
	effectE2,
	effectE3,
	effectE4,
	effectNone
} led_effect_t;

typedef enum {
	sEffectE1 = 0,
	sEffectE2,
	sEffectE3,
	sEffectE4,
	sNone
} led_state_t;

#endif /* LEDMANAGER_H_ */
