# Main Menu Task Documentation

## MainMenu: main menu task
### Overview
The `main_menu_task` is responsible for presenting the main menu to the user and handling the user response. The main menu task will then pass control to the appropriate sub-menu.

### Task Description
- **Task Name:** main_menu_task
- **Priority:** 2
- **Stack Size:** 1000 bytes (250 words)
- **File Location:** `Core/Src/UartManager/UartManager.c`
- **Header File Location:** `Core/Inc/UartManager/UartManager.h`
- **Config File Location:** `Core/Inc/UartManager/Config_UartManager.h`

### Functionality
#### Purpose
The `main_menu_task` performs the following functions:
- Displays the main menu.
- Waits for user input.
- Processes user input.
- Passes control to the selected sub-menu.

#### Code Snippet
```c
void main_menu_task(void *param)
{
	uint32_t msg_addr;
	message_t *msg;
	int option;

	while(1) {

		xQueueSend(q_print, &msg_main_menu, portMAX_DELAY);

		// Wait for menu commands
		xTaskNotifyWait(0, 0, &msg_addr, portMAX_DELAY);
		msg = (message_t*)msg_addr;

		if(msg->len == 1) {
			// Get user option, convert from ASCII to number
			option = msg->payload[0] - 48;
			switch(option) {
				case 0:
					break;
				case 1:
					break;
				case 2:
					break;
				default:
					xQueueSend(q_print, &msg_inv, portMAX_DELAY);
					continue;
			}
		}
		// Handle invalid entry
		else {
			xQueueSend(q_print, &msg_inv, portMAX_DELAY);
			continue;
		}
	}
}
```

## Diagrams

### Data flow diagram
```mermaid
graph TD
    subgraph FreeRTOS Tasks
        A[main_menu_task]
        B[print_task]
    end
    subgraph Queues
        Q1[q_print]
        Q2[q_data]
    end

    A -->|Send main menu message| Q1
    Q1 --> B

    subgraph User Interaction
        C[User Input]
    end

    C -->|Notify input| A

    subgraph Messages
        M1[msg_main_menu]
        M2[msg_inv]
    end

    M1 --> A
    M2 --> A
    A -->|Send invalid entry message| Q1
```

### Sequence diagram

```mermaid
sequenceDiagram
    participant MainMenuTask as main_menu_task
    participant PrintQueue as q_print
    participant User as User
    participant PrintTask as print_task

    MainMenuTask ->> PrintQueue: xQueueSend(msg_main_menu)
    PrintQueue ->> PrintTask: Message to Print
    User ->> MainMenuTask: xTaskNotifyWait(user input)
    MainMenuTask ->> MainMenuTask: Process User Input
    alt Valid Input (0, 1, 2)
        MainMenuTask ->> MainMenuTask: Perform corresponding action
    else Invalid Input
        MainMenuTask ->> PrintQueue: xQueueSend(msg_inv)
        PrintQueue ->> PrintTask: Message to Print
    end


```