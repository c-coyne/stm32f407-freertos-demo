# UartManager Task Documentation

## UartManager: message handler task
### Overview
The `message_handler_task` is responsible for handling all UART transmission and reception operations. This task manages communication between the microcontroller and external devices via the UART interface.

### Task Description
- **Task Name:** message_handler_task
- **Priority:** 2
- **Stack Size:** 1000 bytes (250 words)
- **File Location:** `Core/Src/UartManager/UartManager.c`
- **Header File Location:** `Core/Inc/UartManager/UartManager.h`
- **Config File Location:** `Core/Inc/UartManager/Config_UartManager.h`

### Functionality
#### Purpose
The main application is configured to take care of initialization of the UART peripheral, handling of UART interrupts, and managing the data queue for UART messages.

The `message_handler_task` performs the following functions:
- Waits for notification from the UART interrupt handler
- Receives data from the data queue and processes it accordingly

#### Code Snippet
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

## UartManager: print task
### Overview
The `print_task` is responsible for displaying data to the user via a UART message.

### Task Description
- **Task Name:** print_task
- **Priority:** 2
- **Stack Size:** 1000 bytes (250 words)
- **File Location:** `Core/Src/UartManager/UartManager.c`
- **Header File Location:** `Core/Inc/UartManager/UartManager.h`
- **Config File Location:** `Core/Inc/UartManager/Config_UartManager.h`

### Functionality
#### Purpose
The `print_task` performs the following functions:
- Waits for data to be populated to the print queue (`q_print`)
- Sends data from the print queue to the screen via the STM32 HAL

#### Code Snippet
```c
void print_task(void *param)
{
	uint32_t *msg;

	// Wait for data in the print queue, then send over UART when available
	while(1){
		xQueueReceive(q_print, &msg, portMAX_DELAY);
		HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen((char*)msg),HAL_MAX_DELAY);
	}
}
```

## Diagrams

### Data flow diagram
```mermaid
graph TD
    subgraph FreeRTOS Tasks
        MHT[message_handler_task]
        PT[print_task]
        MMT[main_menu_task]
    end

    subgraph Queues
        Q1[q_print]
        Q2[q_data]
    end

    subgraph Messages
        M1[message_t]
        CMD1[Command]
    end

    subgraph UART Communication
        U1[UART_Transmit]
    end

    MHT -->|Notify process message| PM
    PM -->|Extract command| EC
    EC --> Q2
    Q2 --> EC
    EC --> PM
    PM --> MMT
    MMT -->|Send main menu| Q1
    MMT -->|Notify invalid entry| Q1
    Q1 --> PT
    PT -->|Transmit message over UART| U1
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