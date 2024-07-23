
/*=====================================================================================*\
| Author:   Christopher Coyne                                           July 1st, 2024  |
| --------------------------------------------------------------------------------------|
| Date:     July 1st, 2024                                                              |
| --------------------------------------------------------------------------------------|
| MODULE:     [ MotorManager ]                                                          |
| FILE:       MotorManager.c                                                            |
| --------------------------------------------------------------------------------------|
| DESCRIPTION:                                                                          |
|    The `MotorManager` module is responsible for handling user input and speed         |
|    control for the motor.                                                             |
\*=====================================================================================*/

/****************************************************
 *  Include files                                   *
 ****************************************************/

#include "Config_MotorManager.h"
#include "MotorManager.h"
#include "FreeRTOS.h"
#include "main.h"
#include <string.h>
#include <stdio.h>
#include <math.h>

/****************************************************
 *  Function prototypes                             *
 ****************************************************/

void print_motor_speed(void);
void print_motor_parameters(void);
void initialize_parameters(void);
void print_summary_report(void);
void calculate_average(float data[], int len);
void calculate_sd(float data[], int len);
void split_float_into_ints(int *int_val, int *dec_val, float float_val);

/****************************************************
 *  Messages                                        *
 ****************************************************/

// General system messages
const char *msg_inv_motor = "\n***** Invalid motor control option ******\n";
const char *msg_speed_report = "\n Starting motor speed reporting. Press any key to stop...\n";

// Motor menu
const char *msg_motor_menu = "\n======================================\n"
				  		       "|             Motor Menu             |\n"
						       "======================================\n\n"
							   " Start ---> Start the motor\n"
							   " Stop  ---> Stop the motor\n"
		 	 	 	 	 	   " Algo  ---> Change motion control algorithm\n"
							   " Rec   ---> Start motor speed reporting\n"
							   " Speed ---> Change motor speed\n"
							   " Main  ---> Return to main menu\n\n"
							   " Enter your selection here: ";

// Summary statistics
const char *msg_stat_header = "\n************************************\n"
							    "*        SUMMARY STATISTICS        *\n"
								"*                                  *\n";
const char *msg_stat_footer =   "*                                  *\n"
								"************************************\n";

/****************************************************
 *  Variables                                       *
 ****************************************************/

volatile int32_t encoder_count = 0;
volatile float motor_speed = 0.0f; // Global variable to store speed
volatile uint8_t last_a = 0, last_b = 0;
static int curr_motor_state = MOTOR_INACTIVE;
static int report_counter = 0;
//static float target_speed = 225;

// Summary statistics
static float min_speed = MIN_SPEED_INITIALIZATION;
static float max_speed = MAX_SPEED_INITIALIZATION;
static int duration = 0;
static float average = 0.0;
float standard_dev = 0.0;
float speed_values[1000] = {0};

/****************************************************
 *  Public functions                                *
 ****************************************************/

/*******************************************************************************************************
 * @brief Task to handle motion control of the DC motor.											   *
 *  																								   *
 * This FreeRTOS task handles ...																	   *
 * 																									   *
 * @param param [void*] Parameter passed during task creation (not used in this task).                 *
 * @return void																						   *
 * 																									   *
 * @note The print queue (`q_print`) and other required queues must be initialized.				 	   *
 * @note The task must be notified when a new command is available.									   *
 ******************************************************************************************************/

void motor_task(void *param)
{
	uint32_t msg_addr;
	message_t *msg;

	while(1) {
		// Wait for notification from another task
		xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);

		// Display motor manager menu for the user
		xQueueSend(q_print, &msg_motor_menu, portMAX_DELAY);

		// Wait for the user to make a selection
		xTaskNotifyWait(0, 0, &msg_addr, portMAX_DELAY);
		msg = (message_t*)msg_addr;

		// Process command
		if(msg->len <= 5) {
			if(!strcmp((char*)msg->payload, "Start")) {
				// Set the motor state
				curr_motor_state = MOTOR_ACTIVE;
				// Configure the H-bridge for forward rotation
				HAL_GPIO_WritePin(MOTOR_IN1_GPIO_Port, MOTOR_IN1_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(MOTOR_IN2_GPIO_Port, MOTOR_IN2_Pin, GPIO_PIN_SET);
			}
			else if(!strcmp((char*)msg->payload, "Stop")) {
				// Set the motor state
				curr_motor_state = MOTOR_INACTIVE;
				// Pull both IN1 and IN2 low to stop current flow to the motor
				HAL_GPIO_WritePin(MOTOR_IN1_GPIO_Port, MOTOR_IN1_Pin|MOTOR_IN2_Pin, GPIO_PIN_RESET);
			}
			else if(!strcmp((char*)msg->payload, "Algo")) {
				// execute "Algo" command
			}
			else if(!strcmp((char*)msg->payload, "Rec")) {
				// Set the motor state
				curr_motor_state = MOTOR_SPEED_REPORTING;
				// Notify user of reporting
				xQueueSend(q_print, &msg_speed_report, portMAX_DELAY);
				// Initialize report time counter
				report_counter = 1;
				// Start the motor report timer
				xTimerStart(motor_report_timer, portMAX_DELAY);
			}
			else if(!strcmp((char*)msg->payload, "Speed")) {
				// execute "Speed" command
			}
			else if (!strcmp((char*)msg->payload, "Main")) {
				// Update the system state
				curr_sys_state = sMainMenu;

				// Notify the main menu task
				xTaskNotify(handle_main_menu_task, 0, eNoAction);
			}
			else {
				xQueueSend(q_print, &msg_inv_motor, portMAX_DELAY);
			}
		}
		else {
			// If user input is longer than 5 characters, notify user of invalid response
			xQueueSend(q_print, &msg_inv_motor, portMAX_DELAY);
		}

		// Notify self / motor task if not returning to the main menu
		if (sMotorMenu == curr_sys_state) {
			// Check if speed reporting is active
			if(MOTOR_SPEED_REPORTING == curr_motor_state) {
				// Wait for cancellation from the user before allowing next user input
				xTaskNotifyWait(0, 0, &msg_addr, portMAX_DELAY);
				// Stop the motor report timer
				xTimerStop(motor_report_timer, portMAX_DELAY);
				// Report statistics and reset parameters
				print_summary_report();
				initialize_parameters();
			}
			xTaskNotify(handle_motor_task, 0, eNoAction);
		}
	}
}

void motor_gpio_callback(uint16_t GPIO_Pin)
{
    uint8_t a = HAL_GPIO_ReadPin(ENCODER_A_GPIO_Port, ENCODER_A_GPIO_Pin);
    uint8_t b = HAL_GPIO_ReadPin(ENCODER_B_GPIO_Port, ENCODER_B_GPIO_Pin);

    if (GPIO_Pin == ENCODER_A_GPIO_Pin) {
        if (a != last_a) {
            if (a == b) {
                encoder_count++;
            } else {
                encoder_count--;
            }
            last_a = a;
        }
    } else if (GPIO_Pin == ENCODER_B_GPIO_Pin) {
        if (b != last_b) {
            if (a == b) {
                encoder_count--;
            } else {
                encoder_count++;
            }
            last_b = b;
        }
    }
}

void motor_timer_callback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM7) {
		static int32_t last_encoder_count = 0;
		int32_t delta_count = encoder_count - last_encoder_count;

		// Calculate motor speed in RPM
		motor_speed = (delta_count / (float)ENCODER_COUNTS_PER_REV) * 6000.0 * (1 / (float)ENCODER_QUADRATURE);

		// Update last encoder count for the next period
		last_encoder_count = encoder_count;
	}
}

void motor_report_callback(void)
{
	// Check for min speed
	if(motor_speed < min_speed) {
		min_speed = motor_speed;
	}
	// Check for max speed
	if(motor_speed > max_speed) {
		max_speed = motor_speed;
	}
	// Update time window, add data to array
	speed_values[duration++] = motor_speed;
	// Print current speed
	print_motor_speed();
}

/****************************************************
 *  Private functions                               *
 ****************************************************/

void print_motor_speed(void)
{
	static char showspeed[40];
	static char *speed = showspeed;

	// Separate float into two integers
//	int speed_i = (int)motor_speed;
//	int speed_d = (int)((motor_speed * 100) - (speed_i * 100));
	int speed_i = 0;
	int speed_d = 0;
	split_float_into_ints(&speed_i, &speed_d, motor_speed);

	// Display speed in RPM
	sprintf((char*)showspeed, " [%03ds] Motor speed: %03d.%02d RPM\n", report_counter++, speed_i, speed_d);
	xQueueSend(q_print, &speed, portMAX_DELAY);
}

void print_motor_parameters(void)
{

}

void initialize_parameters(void)
{
	report_counter = 0;
	duration = 0;
	min_speed = MIN_SPEED_INITIALIZATION;
	max_speed = MAX_SPEED_INITIALIZATION;
	average = 0;
	standard_dev = 0;
}

void print_summary_report(void)
{
	// Send statistics header message
	xQueueSend(q_print, &msg_stat_header, portMAX_DELAY);

	// Calculate statistics
	calculate_average(speed_values, duration);
	calculate_sd(speed_values, duration);

	// Convert floats into two integer values for display
	int min_speed_i = 0, min_speed_d = 0;
	int max_speed_i = 0, max_speed_d = 0;
	int average_i = 0, average_d = 0;
	int standard_dev_i = 0, standard_dev_d = 0;
	split_float_into_ints(&min_speed_i, &min_speed_d, min_speed);
	split_float_into_ints(&max_speed_i, &max_speed_d, max_speed);
	split_float_into_ints(&average_i, &average_d, average);
	split_float_into_ints(&standard_dev_i, &standard_dev_d, standard_dev);

	// Print results
	static char showstats[250];
	static char *stats = showstats;
	sprintf((char*)showstats,   "* Min speed:          %03d.%02d RPM   *"
							  "\n* Max speed:          %03d.%02d RPM   *"
			                  "\n* Average speed:      %03d.%02d RPM   *"
			                  "\n* Standard deviation: %03d.%02d RPM   *\n",
							  min_speed_i, min_speed_d, max_speed_i, max_speed_d, average_i, average_d, standard_dev_i, standard_dev_d);
	xQueueSend(q_print, &stats, portMAX_DELAY);

	// Send statistics footer message
	xQueueSend(q_print, &msg_stat_footer, portMAX_DELAY);
}

void calculate_average(float data[], int len)
{
	float sum = 0.0;
	for(int i = 0; i < len; ++i) {
		sum += data[i];
	}
	average = sum / len;
}

// Note: calculate_average must be run first
void calculate_sd(float data[], int len)
{
	for(int i = 0; i < len; i++) {
		standard_dev += ( (data[i]-average) * (data[i]-average) );
	}
	standard_dev = sqrt(standard_dev / len);
}

void split_float_into_ints(int *int_val, int *dec_val, float float_val)
{
	// Separate float into two integers
	*int_val = (int)float_val;
	*dec_val = (int)((float_val * 100) - (*int_val * 100));
}
