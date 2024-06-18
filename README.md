# STM32F407 FreeRTOS Demo Project

## Overview
This project is intended to demonstrate a variety of the features available within FreeRTOS. It's intentionally limited to the STM32F407 discovery board to minimize the amount of hardware required to set up the project and run. The application is rather simple, given that we're just exploring the development board and not necessarily "achieving" anything functional. Nonetheless, the project sets up multiple tasks to interact with various peripherals (ex. GPIO, UART, and RTC) and therefore hopefully serves as a somewhat comprehensive example of FreeRTOS in action, highlighting task management, inter-task communication, and peripheral interfacing.

## Features
- **Task Management:** Demonstrates the creation and management of multiple FreeRTOS tasks.
- **Inter-Task Communication:** Utilizes queues, semaphores, and other FreeRTOS synchronization mechanisms.
- **Peripheral Control:** Interfaces with GPIO, UART, and RTC peripherals.

## Hardware and Software Requirements
- **Hardware:** STM32F407 Discovery Board, FTDI USB-to-UART converter
- **Software:** STM32CubeIDE, FreeRTOS

## Project Structure
```
FreeRTOSDemoProject/
├── Binaries/
├── Includes/
├── Core/
│ ├── Inc/
│ │ ├── FreeRTOSConfig.h
│ │ ├── main.h
│ │ ├── stm32f4xx_hal_conf.h
│ │ └── stm32f4xx_it.h
│ ├── Src/
│ │ ├── UartManager/
| | | ├── Config_UartManager.h
| | | ├── UartManager.h
| | | └── UartManager.c
│ │ ├── main.c
│ │ ├── stm32f4xx_hal_msp.c
│ │ ├── stm32f4xx_hal_timebase_tim.c
│ │ ├── stm32f4xx_it.c
│ │ ├── syscalls.c
│ │ ├── sysmem.c
│ │ └── system_stm32f4xx.c
| ├── Startup/
├── Drivers/
│ ├── CMSIS/
│ └── STM32F4XX_HAL_Driver/
├── ThirdParty/
│ └── FreeRTOS/
├── Debug/
├── Docs/
│ ├── Overview/
│ │ └── README.md
│ ├── Diagrams/
│ ├── Communication/
│ └── Tasks/
│ │ └── UartManager.md
└── README.md
```

## Getting Started

### Prerequisites
- **STM32CubeIDE:** Install from [STMicroelectronics](https://www.st.com/en/development-tools/stm32cubeide.html).
- **FreeRTOS:** Ensure FreeRTOS is included in your project setup. In my case, I set this up manually, but you can also use the STM32CubeIDE to semi-automatically configure FreeRTOS for you.

### Installation
1. **Clone the repository:**
   ```bash
   git clone https://github.com/yourusername/STM32F407_FreeRTOS_Project.git
2. **Open the project in STM32CubeIDE.**
3. **Build the project by clicking on the build icon or using the menu Project -> Build All.**
4. **Flash the firmware to your STM32F407 Discovery board using the debug icon or menu Run -> Debug.**

## Task Descriptions
### UART Manager

    File: Core/Src/UartManager/UartManager.c
    Description: Handles UART transmission and reception.
    Documentation: UartManager.md

### Communication Mechanisms

    Queues: Used for task-to-task communication.
        Documentation: TODO
    Semaphores: Used for synchronization between tasks.
        Documentation: TODO

### Diagrams

    Block Diagram: BlockDiagram.png
    Sequence Diagram: SequenceDiagram.png

## License

This project is licensed under the [MIT License](https://opensource.org/license/MIT).

## Contact

Christopher Coyne: christopher.w.coyne@gmail.com  
Project Link: https://github.com/c-coyne/stm32f407-freertos-demo

## Acknowledgements

    FreeRTOS: FreeRTOS.org
    STM32CubeIDE: STMicroelectronics

