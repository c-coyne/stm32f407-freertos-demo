
/*=====================================================================================*\
| Author:   Christopher Coyne                                          June 19th, 2024  |
| --------------------------------------------------------------------------------------|
| Date:     June 19th, 2024                                                             |
| --------------------------------------------------------------------------------------|
| MODULE:     [ LedManager ]                                                            |
| FILE:       Config_LedManager.h                                                       |
| --------------------------------------------------------------------------------------|
| DESCRIPTION:                                                                          |
|    The `LedManager` module is responsible for handling user input and all effects     |
|    pertaining to the on-board LEDs.                                                   |
\*=====================================================================================*/

#ifndef CONFIG_LEDMANAGER_H_
#define CONFIG_LEDMANAGER_H_

/****************************************************
 *  Macros                                          *
 ****************************************************/

// Number of configured software timers
#define NUM_LED_TIMERS				4

// LED states
#define LED_OFF						GPIO_PIN_RESET
#define LED_ON						GPIO_PIN_SET

// LED groups
#define LED_EVEN					0
#define LED_ODD						1

// LEDs
// Top
#define ORANGE_LED_PORT				LD3_GPIO_Port
#define ORANGE_LED_PIN				LD3_Pin
// Left
#define GREEN_LED_PORT				LD4_GPIO_Port
#define GREEN_LED_PIN				LD4_Pin
// Bottom
#define BLUE_LED_PORT				LD6_GPIO_Port
#define BLUE_LED_PIN				LD6_Pin
// Right
#define RED_LED_PORT				LD5_GPIO_Port
#define RED_LED_PIN					LD5_Pin

// FreeRTOS task wait time [ms]
#define EVENT_GROUP_WAIT_TIME		100
#define RTC_SEMAPHORE_WAIT_TIME		10

#endif /* CONFIG_LEDMANAGER_H_ */
