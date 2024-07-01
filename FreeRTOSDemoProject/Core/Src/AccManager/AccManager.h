
/*=====================================================================================*\
| Author:   Christopher Coyne                                          June 24th, 2024  |
| --------------------------------------------------------------------------------------|
| Date:     June 24th, 2024                                                             |
| --------------------------------------------------------------------------------------|
| MODULE:     [ AccManager ]                                                            |
| FILE:       AccManager.h                                                              |
| --------------------------------------------------------------------------------------|
| DESCRIPTION:                                                                          |
|    The `AccManager` module is responsible for handling user input and all effects     |
|    pertaining to the on-board accelerometer.                                          |
\*=====================================================================================*/

#ifndef ACCMANAGER_H_
#define ACCMANAGER_H_

/****************************************************
 *  Include files                                   *
 ****************************************************/

#include "FreeRTOS.h"
#include "main.h"

/****************************************************
 *  Public functions                                *
 ****************************************************/

void acc_task(void* param);
void accelerometer_init(void);

#endif /* ACCMANAGER_H_ */
