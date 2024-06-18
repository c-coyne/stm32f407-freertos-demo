/*
 * UartManager.h
 *
 *  Created on: Jun 17, 2024
 *      Author: chris
 */

#ifndef UARTMANAGER_H_
#define UARTMANAGER_H_

/****************************************************
 *  Include files                                   *
 ****************************************************/

#include <stdint.h>

/****************************************************
 *  Public functions                                *
 ****************************************************/

void message_handler_task(void *param);
void print_task(void *param);

typedef struct
{
	uint8_t payload[10];
	uint32_t len;
}message_t;

#endif /* UARTMANAGER_H_ */
