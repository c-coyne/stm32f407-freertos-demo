# UartManager Task Documentation

## Overview
The `UartManager` task is responsible for handling all UART transmission and reception operations. This task manages communication between the microcontroller and external devices via the UART interface.

## Task Description
- **Task Name:** UartManager
- **Priority:** [Specify priority]
- **Stack Size:** [Specify stack size]
- **File Location:** `Core/Src/UartManager/UartManager.c`
- **Header File Location:** `Core/Inc/UartManager/UartManager.h`
- **Config File Location:** `Core/Inc/UartManager/Config_UartManager.h`

## Functionality
### Purpose
The `UartManager` task performs the following functions:
- Initializes the UART peripheral.
- Sends and receives data over UART.
- Handles UART interrupts.
- Manages a queue for UART messages.

### Code Snippet
```c
void message_handler_task(void *param)
{
	BaseType_t ret;
	message_t msg;

	while(1) {

		// Wait until task is notified
		ret = xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);

		if(pdTRUE == ret) {
			// Process the message stored in the input data queue
			process_message(&msg);
		}

	}
}
```

## Diagrams

### Data flow diagram
```mermaid
flowchart TD
    id1[UART interrupt handler] --> id2[UART 'user_data' reception buffer]
    id2 --> id3[Queue: q_data]
    id3 --> message_handler_task
    id1 -->|xTaskNotifyFromISR| message_handler_task
    message_handler_task --> process_message
    process_message --> id4[Queue: q_print]
    id4 --> print_task
    print_task --> HAL_UART_transmit
```

### Sequence diagram

```mermaid
sequenceDiagram
    
    participant UI as UART Interrupt
    participant M as Main
    participant STM as STM HAL
    box Green UartManager
    participant MHT as message_handler_task
    participant PT as print_task
    end

    UI->>M: Rx byte
    M->>M: Process interrupt (HAL_UART_RxCpltCallback)
    alt Non-newline character data is available
        M->>M: Enqueue data byte
        M->>STM: Set UART for next Rx byte (HAL_UART_Receive_IT)
    else '\n' byte received
        M->>M: Format data queue message
        M->>MHT: Notify message handler (xTaskNotifyFromISR)
        MHT->>MHT: Process message (process_message)
        MHT->>PT: Load default 'message received' message in print queue (xQueueSend)
        PT->>STM: HAL_UART_Transmit
    end

```