
/*=====================================================================================*\
| Author:   Christopher Coyne                                          June 24th, 2024  |
| --------------------------------------------------------------------------------------|
| Date:     June 24th, 2024                                                             |
| --------------------------------------------------------------------------------------|
| MODULE:     [ AccManager ]                                                            |
| FILE:       Config_AccManager.h                                                       |
| --------------------------------------------------------------------------------------|
| DESCRIPTION:                                                                          |
|    The `AccManager` module is responsible for handling user input and all effects     |
|    pertaining to the on-board accelerometer.                                          |
\*=====================================================================================*/

#ifndef SRC_ACCMANAGER_CONFIG_ACCMANAGER_H_
#define SRC_ACCMANAGER_CONFIG_ACCMANAGER_H_

/****************************************************
 *  Macros                                          *
 ****************************************************/

// LSM303DLHC Accelerometer I2C address
#define ACC_I2C_ADDRESS			0x32

// LSM303DLHC Registers
#define LSM303DLHC_CTRL_REG1_A 	0x20
#define LSM303DLHC_OUT_X_L_A   	0x28 | 0x80  // Auto-increment enabled
#define ACC_X_ADDR				LSM303DLHC_OUT_X_L_A
#define ACC_Y_ADDR				LSM303DLHC_OUT_Y_L_A
#define ACC_Z_ADDR				LSM303DLHC_OUT_Z_L_A

#endif /* SRC_ACCMANAGER_CONFIG_ACCMANAGER_H_ */
