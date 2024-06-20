
/*=====================================================================================*\
| Author:   Christopher Coyne                                          June 20th, 2024  |
| --------------------------------------------------------------------------------------|
| Date:     June 20th, 2024                                                             |
| --------------------------------------------------------------------------------------|
| MODULE:     [ RtcManager ]                                                            |
| FILE:       RtcManager.h                                                              |
| --------------------------------------------------------------------------------------|
| DESCRIPTION:                                                                          |
|    The `RtcManager` module is responsible for handling user input and configuration   |
|    of the real time clock (RTC).                                                      |
\*=====================================================================================*/

#ifndef SRC_RTCMANAGER_RTCMANAGER_H_
#define SRC_RTCMANAGER_RTCMANAGER_H_

/****************************************************
 *  Include files                                   *
 ****************************************************/

#include "FreeRTOS.h"

void rtc_task(void *param);

#endif /* SRC_RTCMANAGER_RTCMANAGER_H_ */
