# Queues

## Overview

This document provides an overview of the queues used in the project to handle UART communication. There are two main queues:

1. **Data Queue (`q_data`)**: Used to store data received from UART.
2. **Print Queue (`q_print`)**: Used to store messages to be transmitted via UART.

## Queue: `q_data`

### Purpose
The `q_data` queue stores incoming data bytes received from the UART interrupt handler. It ensures that data can be processed asynchronously by the `message_handler_task`.

### Configuration
- **Type**: Queue
- **Length**: 10 items
- **Item Size**: 1 byte (sizeof(char))

### Usage
- **Producer**: UART Interrupt Handler (`HAL_UART_RxCpltCallback`)
- **Consumer**: `message_handler_task`

### Code Snippets

#### Initialization
```c
q_data = xQueueCreate(10, sizeof(char));
configASSERT(NULL != q_data);
```

#### Producer: UART Interrupt Handler
```c
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    uint8_t dummy;
    if (!xQueueIsQueueFullFromISR(q_data)) {
        xQueueSendFromISR(q_data, (void*)&user_data, NULL);
    } else {
        if (user_data == '\n') {
            xQueueReceiveFromISR(q_data, (void*)&dummy, NULL);
            xQueueSendFromISR(q_data, (void*)&user_data, NULL);
        }
    }
    if (user_data == '\n') {
        xTaskNotifyFromISR(handle_message_handler_task, 0, eNoAction, NULL);
    }
    HAL_UART_Receive_IT(&huart2, (uint8_t*)&user_data, 1);
}
```

#### Consumer: message_handler_task
```c
void message_handler_task(void *param)
{
    BaseType_t ret;
    message_t msg;

    while (1) {
        ret = xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
        if (pdTRUE == ret) {
            process_message(&msg);
        }
    }
}
```

## Queue: `q_print`

### Purpose
The `q_print` queue holds messages that are ready to be transmitted over UART. It allows the `print_task` to send data asynchronously.

### Configuration
- **Type**: Queue
- **Length**: 10 items
- **Item Size**: size_t

### Usage
- **Producer**: `process_message`
- **Consumer**: `print_task`

### Code Snippets

#### Initialization
```c
q_print = xQueueCreate(10, sizeof(size_t));
configASSERT(NULL != q_print);
```

#### Producer: `process_message`
```c
void process_message(message_t *msg) {
    xQueueSend(q_print, &msg_rcv, portMAX_DELAY);
}
```

#### Consumer: `print_task`
```c
void print_task(void *param)
{
    uint32_t *msg;

    while (1) {
        xQueueReceive(q_print, &msg, portMAX_DELAY);
        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen((char*)msg), HAL_MAX_DELAY);
    }
}
```

## Queue Interaction

### Data Flow

1. **Data Reception:** UART interrupt receives data byte and pushes it onto the `q_data` queue.
2. **Message Handling:** `message_handler_task` processes the received data from `q_data`.
3. **Message Processing:** The processed message is then sent to the `q_print` queue.
4. **Data Transmission:** `print_task` reads the message from `q_print` and transmits it via UART.