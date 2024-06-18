
/*=====================================================================================*\
| Author:   Christopher Coyne                                          June 18th, 2024  |
| --------------------------------------------------------------------------------------|
| Date:     June 18th, 2024                                                             |
| --------------------------------------------------------------------------------------|
| MODULE:     [ UartManager ]                                                           |
| FILE:       Config_UartManager.h                                                      |
| --------------------------------------------------------------------------------------|
| DESCRIPTION:                                                                          |
|    The `UartManager` task is responsible for handling all UART transmission and       |
|    reception operations. This task manages communication between the microcontroller  |
|    and external devices via the UART interface.                                       |
| --------------------------------------------------------------------------------------|
\*=====================================================================================*/

#ifndef CONFIG_UARTMANAGER_H_
#define CONFIG_UARTMANAGER_H_

/****************************************************
 *  Messages                                        *
 ****************************************************/

const char *msg_inv = "**** Invalid option ****\n";
const char *msg_rcv = " Success: message received\n";

#endif /* CONFIG_UARTMANAGER_H_ */
