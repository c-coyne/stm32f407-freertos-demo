
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
#include <stdlib.h>
#include <ctype.h>

/****************************************************
 *  Function prototypes                             *
 ****************************************************/

void print_motor_speed(void);
void initialize_parameters(void);
void print_motor_on_report(void);
void print_summary_report(void);
void calculate_average(float data[], int len);
void calculate_sd(float data[], int len);
void split_float_into_ints(int *int_val, int *dec_val, float float_val, int dec_places);
void set_pwm_duty_cycle(TIM_HandleTypeDef *htim, uint32_t channel, uint8_t duty_cycle_percent);
float pid_controller(float setpoint, float measured_value);
int isNumeric(const char *str);
int parse_param_string(message_t *msg);

/****************************************************
 *  Messages                                        *
 ****************************************************/

// General system messages
const char *msg_inv_motor = "\n***** Invalid motor control option ******\n";
const char *msg_speed_report = "\n Starting motor speed reporting. Press any key to stop...\n\n";

// Motor menu
const char *msg_motor_menu = "\n======================================\n"
				  		       "|             Motor Menu             |\n"
						       "======================================\n\n"
							   " Start ---> Start the motor\n"
							   " Stop  ---> Stop the motor\n"
		 	 	 	 	 	   " Algo  ---> Change motion control algorithm\n"
							   " Param ---> Change algorithm parameter\n"
							   " Rec   ---> Start motor speed reporting\n"
							   " Speed ---> Change target speed\n"
							   " Main  ---> Return to main menu\n\n"
							   " Enter your selection here: ";

// Algorithm
const char *msg_motor_algo = "\n Enter algorithm here (0 = None, 1 = PID): ";
const char *msg_valid_algo = "\n Confirmed: motor speed control algorithm updated\n";
const char *msg_inv_algo = "\n***** Invalid algorithm selection *****\n";

// Parameters
const char *msg_valid_param = "\n Confirmed: algorithm parameter updated\n";
const char *msg_motor_param = "\n Enter parameter (KpX.XXX, KdX.XXX, KeX.XXX): ";
const char *msg_inv_param = "\n***** Invalid parameter selection *****\n";

// Speed
const char *msg_motor_speed = "\n Enter new motor speed (RPM): ";
const char *msg_motor_speed_max = "\n Selection exceeds threshold.\n";
const char *msg_valid_speed = "\n Confirmed: motor speed updated\n";
const char *msg_inv_speed = "\n***** Invalid speed selection *****\n";

// Summary statistics
const char *msg_stat_header = "************************************\n"
							  "*        SUMMARY STATISTICS        *\n"
							  "*                                  *\n";
const char *msg_stat_footer = "*                                  *\n"
							  "************************************\n";
const char *msg_motor_on_header = "\n************************************\n"
	    							"*            MOTOR START           *\n"
									"*                                  *\n";
const char *msg_motor_on_footer =   "*                                  *\n"
									"************************************\n";

/****************************************************
 *  Variables                                       *
 ****************************************************/

volatile int32_t encoder_count = 0;
volatile float motor_speed = 0.0f; // Global variable to store speed
volatile uint8_t last_a = 0, last_b = 0;
static int curr_motor_state = MOTOR_INACTIVE;
static int report_counter = 0;

// Summary statistics
static float min_speed = MIN_SPEED_INITIALIZATION;
static float max_speed = MAX_SPEED_INITIALIZATION;
static int duration = 0;
static float average = 0.0;
float standard_dev = 0.0;
float speed_values[1000] = {0};

// PID parameters
static float Kp = 0.5;
static float Ki = 0.000;
static float Kd = 0.000;
static float duty_cycle = 50.0f; // Initial duty cycle (in percentage)
static volatile float target_speed = 225.0; // Desired motor speed in RPM
static float integral = 0.0;
static float last_error = 0.0;
static float dt = 0.01; // Time step in seconds (adjust as needed)
static volatile motor_algo_t motor_algo = 1; // 0 for no algorithm, 1 for PID

/****************************************************
 *  Public functions                                *
 ****************************************************/

/*******************************************************************************************************
 * @brief Task to handle motion control of the DC motor.											   *
 * 																									   *
 * This FreeRTOS task handles the user interface and command processing for controlling the DC motor.  *
 * It waits for notifications and processes commands to start, stop, configure, and report on the 	   *
 * motor.																							   *
 * 																									   *
 * @param param [void*] Parameter passed during task creation (not used in this task).				   *
 * @return void																						   *
 * 																									   *
 * @note The print queue (`q_print`) and other required queues must be initialized.					   *
 * @note The task must be notified when a new command is available.									   *
 ******************************************************************************************************/

void motor_task(void *param)
{
	uint32_t msg_addr;
	message_t *msg;

	while(1) {
		// Wait for notification from another task
		xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);

		switch(curr_sys_state) {

			case sMotorMenu:
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
						HAL_GPIO_WritePin(MOTOR_IN1_GPIO_Port, MOTOR_IN1_Pin, GPIO_PIN_SET);
						HAL_GPIO_WritePin(MOTOR_IN2_GPIO_Port, MOTOR_IN2_Pin, GPIO_PIN_RESET);
					}
					else if(!strcmp((char*)msg->payload, "Stop")) {
						// Set the motor state
						curr_motor_state = MOTOR_INACTIVE;
						// Pull both IN1 and IN2 low to stop current flow to the motor
						HAL_GPIO_WritePin(MOTOR_IN1_GPIO_Port, MOTOR_IN1_Pin|MOTOR_IN2_Pin, GPIO_PIN_RESET);
					}
					else if(!strcmp((char*)msg->payload, "Algo")) {
						// Update the system state
						curr_sys_state = sMotorAlgo;
						// Prompt user for algorithm selection
						xQueueSend(q_print, &msg_motor_algo, portMAX_DELAY);
					}
					else if(!strcmp((char*)msg->payload, "Param")) {
						// Update the system state
						curr_sys_state = sMotorParam;
						// Prompt user for algorithm selection
						xQueueSend(q_print, &msg_motor_param, portMAX_DELAY);
					}
					else if(!strcmp((char*)msg->payload, "Rec")) {
						// Set the motor state
						curr_motor_state = MOTOR_SPEED_REPORTING;
						// Display parameters
						print_motor_on_report();
						// Notify user of reporting
						xQueueSend(q_print, &msg_speed_report, portMAX_DELAY);
						// Initialize report time counter
						report_counter = 1;
						// Start the motor report timer
						xTimerStart(motor_report_timer, portMAX_DELAY);
					}
					else if(!strcmp((char*)msg->payload, "Speed")) {
						// Update the system state
						curr_sys_state = sMotorSpeed;
						// Prompt user for new speed
						xQueueSend(q_print, &msg_motor_speed, portMAX_DELAY);
					}
					else if (!strcmp((char*)msg->payload, "Main")) {
						// Update the system state
						curr_sys_state = sMainMenu;

						// Notify the main menu task
						xTaskNotify(handle_main_menu_task, 0, eNoAction);
					}
					else {
						// Update the system state
						curr_sys_state = sMotorMenu;
						curr_motor_state = MOTOR_INVALID_INPUT;

						// Notify user of invalid input
						xQueueSend(q_print, &msg_inv_motor, portMAX_DELAY);
					}
				}
				else {
					// Update the system state
					curr_sys_state = sMotorMenu;
					curr_motor_state = MOTOR_INVALID_INPUT;

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
				else if (sMotorAlgo == curr_sys_state || sMotorSpeed == curr_sys_state || sMotorParam == curr_sys_state) {
					xTaskNotify(handle_motor_task, 0, eNoAction);
				}
				break;
			case sMotorAlgo:
				// Wait for the user to make a selection
				xTaskNotifyWait(0, 0, &msg_addr, portMAX_DELAY);
				msg = (message_t*)msg_addr;

				// Process command
				if(msg->len <= 1) {
					if(!strcmp((char*)msg->payload, "0")) { 					// None
						// Disable all algorithms control
						motor_algo = 0;
						xQueueSend(q_print, &msg_valid_algo, portMAX_DELAY);
					}
					else if(!strcmp((char*)msg->payload, "1")) { 				// PID control
						// Enable PID control
						motor_algo = 1;
						xQueueSend(q_print, &msg_valid_algo, portMAX_DELAY);
					}
					else {
						xQueueSend(q_print, &msg_inv_algo, portMAX_DELAY);
					}
				}
				else {
					// If something longer than 1 digit is entered, this is incorrect
					xQueueSend(q_print, &msg_inv_algo, portMAX_DELAY);
				}
				// Update system state
				curr_sys_state = sMotorMenu;
				// Send control back to motor task main menu
				xTaskNotify(handle_motor_task, 0, eNoAction);
				break;
			case sMotorParam:
				// Wait for the user to make a selection
				xTaskNotifyWait(0, 0, &msg_addr, portMAX_DELAY);
				msg = (message_t*)msg_addr;

				// Process command
				if(parse_param_string(msg)) {
					xQueueSend(q_print, &msg_valid_param, portMAX_DELAY);
				}
				else {
					// If invalid entry, notify the user
					xQueueSend(q_print, &msg_inv_param, portMAX_DELAY);
				}
				// Update system state
				curr_sys_state = sMotorMenu;
				// Send control back to motor task main menu
				xTaskNotify(handle_motor_task, 0, eNoAction);
				break;
			case sMotorSpeed:
				// Wait for the user to make a selection
				xTaskNotifyWait(0, 0, &msg_addr, portMAX_DELAY);
				msg = (message_t*)msg_addr;

				// Process command
				if(msg->len <= 6) {
					if(isNumeric((char*)msg->payload)) {
						// Convert speed from string to int
						target_speed = strtof((char*)msg->payload, NULL);
						if(target_speed > MAX_MOTOR_SPEED) {
							// Set motor speed to configured threshold
							target_speed = MAX_MOTOR_SPEED;
							// Notify user that selection exceeds maximum RPM threshold
							xQueueSend(q_print, &msg_motor_speed_max, portMAX_DELAY);
							// Notify user of current threshold
							static char maxspeed[40];
							static char *max_speed = maxspeed;
							// Display speed in RPM
							sprintf((char*)max_speed, " Motor speed set to: %03d RPM\n", (int)MAX_MOTOR_SPEED);
							xQueueSend(q_print, &max_speed, portMAX_DELAY);
						}
						else {
							xQueueSend(q_print, &msg_valid_speed, portMAX_DELAY);
						}
					}
					else {
						xQueueSend(q_print, &msg_inv_speed, portMAX_DELAY);
					}
				}
				else {
					// If something longer than 6 digits is entered, this is incorrect
					xQueueSend(q_print, &msg_inv_speed, portMAX_DELAY);
				}
				// Update system state
				curr_sys_state = sMotorMenu;
				// Send control back to motor task main menu
				xTaskNotify(handle_motor_task, 0, eNoAction);
				break;
			default:
				break;
		}
	}
}

/*******************************************************************************************************
 * @brief Callback for motor GPIO interrupt.														   *
 * 																									   *
 * This function handles the GPIO interrupt for motor encoders. It reads the encoder pins and updates  *
 * the encoder count based on the state changes. This is used to keep track of the motor's speed.	   *
 * 																									   *
 * @param GPIO_Pin The pin that triggered the interrupt.											   *
 * @return void																						   *
 ******************************************************************************************************/

void motor_gpio_callback(uint16_t GPIO_Pin)
{
    uint8_t a = HAL_GPIO_ReadPin(ENCODER_A_GPIO_Port, ENCODER_A_GPIO_Pin);
    uint8_t b = HAL_GPIO_ReadPin(ENCODER_B_GPIO_Port, ENCODER_B_GPIO_Pin);

    if (GPIO_Pin == ENCODER_A_GPIO_Pin) {
        if (a != last_a) {
            if (a == b) {
                encoder_count--;
            } else {
                encoder_count++;
            }
            last_a = a;
        }
    } else if (GPIO_Pin == ENCODER_B_GPIO_Pin) {
        if (b != last_b) {
            if (a == b) {
                encoder_count++;
            } else {
                encoder_count--;
            }
            last_b = b;
        }
    }
}

/*******************************************************************************************************
 * @brief Callback for motor timer interrupt.														   *
 * 																									   *
 * This function runs every 10 ms and handles the timer interrupt for motor control. It calculates     *
 * the motor speed based on the encoder count and updates the PWM duty cycle using a PID controller    *
 * to achieve the target speed.																		   *
 * 																									   *
 * @param htim Pointer to the timer handle.															   *
 * @return void																						   *
 ******************************************************************************************************/

void motor_timer_callback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM7) {
		static int32_t last_encoder_count = 0;
		int32_t delta_count = encoder_count - last_encoder_count;

		// Calculate motor speed in RPM
		motor_speed = (delta_count / (float)ENCODER_COUNTS_PER_REV) * 6000.0 * (1 / (float)ENCODER_QUADRATURE);

		// Update last encoder count for the next period
		last_encoder_count = encoder_count;

		// PID control
		float new_duty_cycle = pid_controller(target_speed, motor_speed);
		set_pwm_duty_cycle(&htim3, TIM_CHANNEL_1, (uint8_t)new_duty_cycle);
	}
}

/*******************************************************************************************************
 * @brief Callback for motor report.																   *
 * 																									   *
 * This function runs every 1 sec to update motor statistics. It checks and updates the minimum and    *
 * maximum motor speeds, adds the current speed to an array for statistical analysis, and prints the   *
 * current motor speed.																				   *
 * 																									   *
 * @return void																						   *
 ******************************************************************************************************/

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

/*******************************************************************************************************
 * @brief Prints the motor speed.																	   *
 * 																									   *
 * This function formats the current motor speed into a human-readable string and sends it to the      *
 * print queue. It separates the floating-point motor speed into integer and decimal components for    *
 * display.																							   *
 * 																									   *
 * @return void																						   *
 ******************************************************************************************************/

void print_motor_speed(void)
{
	static char showspeed[40];
	static char *speed = showspeed;

	// Separate float into two integers
	int speed_i = 0;
	int speed_d = 0;
	split_float_into_ints(&speed_i, &speed_d, motor_speed, 2);

	// Display speed in RPM
	sprintf((char*)showspeed, " [%03ds] Motor speed: %03d.%02d RPM\n", report_counter++, speed_i, speed_d);
	xQueueSend(q_print, &speed, portMAX_DELAY);
}

/*******************************************************************************************************
 * @brief Initializes motor and statistical parameters.												   *
 * 																									   *
 * This function sets the initial values for parameters related to motor statistics, including report  *
 * counter, duration, minimum speed, maximum speed, average speed, and standard deviation.			   *
 * 																									   *
 * @return void																						   *
 ******************************************************************************************************/

void initialize_parameters(void)
{
	report_counter = 0;
	duration = 0;
	min_speed = MIN_SPEED_INITIALIZATION;
	max_speed = MAX_SPEED_INITIALIZATION;
	average = 0;
	standard_dev = 0;
}

/*******************************************************************************************************
 * @brief Prints a report of the motor's parameters upon starting a recording of motor speed.		   *
 * 																									   *
 * This function sends a formatted report containing the target speed, and PID controller parameters   *
 * (Kp, Ki, Kd) to the print queue. It separates the floating-point values into integer components for *
 * display and sends the formatted string.															   *
 * 																									   *
 * @return void																						   *
 ******************************************************************************************************/

void print_motor_on_report(void)
{
	// Send statistics header message
	xQueueSend(q_print, &msg_motor_on_header, portMAX_DELAY);

	// Convert floats into two integer values for display
	int target_speed_i = 0, target_speed_d = 0;
	int kp_i = 0, kp_d = 0;
	int ki_i = 0, ki_d = 0;
	int kd_i = 0, kd_d = 0;
	split_float_into_ints(&target_speed_i, &target_speed_d, target_speed, 2);
	split_float_into_ints(&kp_i, &kp_d, Kp, 3);
	split_float_into_ints(&ki_i, &ki_d, Ki, 3);
	split_float_into_ints(&kd_i, &kd_d, Kd, 3);

	// Print results
	static char showparams[250];
	static char *params = showparams;
	sprintf((char*)showparams,   "* Target speed:      %03d.%02d  RPM   *"
							   "\n* Kp:                  %01d.%03d       *"
							   "\n* Ki:                  %01d.%03d       *"
			                   "\n* Kd:                  %01d.%03d       *\n",
							   target_speed_i, target_speed_d, kp_i, kp_d, ki_i, ki_d, kd_i, kd_d);
	xQueueSend(q_print, &params, portMAX_DELAY);

	// Send statistics footer message
	xQueueSend(q_print, &msg_motor_on_footer, portMAX_DELAY);
}

/*******************************************************************************************************
 * @brief Prints a summary report of statistical calculations.										   *
 * 																									   *
 * This function sends a formatted summary report containing elapsed time, minimum speed, maximum 	   *
 * speed, average speed, and standard deviation to the print queue. It first calculates the average	   *
 * and standard deviation of speed values, splits the resulting floats into integer components, and    *
 * then formats and sends the statistics for printing.												   *
 * 																									   *
 * @return void																						   *
 ******************************************************************************************************/

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
	split_float_into_ints(&min_speed_i, &min_speed_d, min_speed, 2);
	split_float_into_ints(&max_speed_i, &max_speed_d, max_speed, 2);
	split_float_into_ints(&average_i, &average_d, average, 2);
	split_float_into_ints(&standard_dev_i, &standard_dev_d, standard_dev, 2);

	// Print results
	static char showstats[250];
	static char *stats = showstats;
	sprintf((char*)showstats,   "* Elapsed time:       %06d sec   *"
							  "\n* Min speed:          %03d.%02d RPM   *"
							  "\n* Max speed:          %03d.%02d RPM   *"
			                  "\n* Average speed:      %03d.%02d RPM   *"
			                  "\n* Standard deviation: %03d.%02d RPM   *\n",
							  duration, min_speed_i, min_speed_d, max_speed_i, max_speed_d, average_i, average_d, standard_dev_i, standard_dev_d);
	xQueueSend(q_print, &stats, portMAX_DELAY);

	// Send statistics footer message
	xQueueSend(q_print, &msg_stat_footer, portMAX_DELAY);
}

/*******************************************************************************************************
 * @brief Calculates the average value of an array of floats.										   *
 * 																									   *
 * This function computes the average of an array of floating-point numbers and stores the result in   *
 * the global variable 'average'.																	   *
 * 																									   *
 * @param data [float[]] Array of float data to calculate the average of.							   *
 * @param len [int] Length of the data array.														   *
 * @return void																						   *
 ******************************************************************************************************/

void calculate_average(float data[], int len)
{
	float sum = 0.0;
	for(int i = 0; i < len; ++i) {
		sum += data[i];
	}
	average = sum / len;
}

/*******************************************************************************************************
 * @brief Calculates the standard deviation of an array of floats.									   *
 * 																									   *
 * This function computes the standard deviation of an array of floating-point numbers and stores the  *
 * result in the global variable 'standard_dev'. The function assumes that the 'calculate_average' 	   *
 * function has been called first to compute the global 'average'.									   *
 * 																									   *
 * @param data [float[]] Array of float data to calculate the standard deviation of.				   *
 * @param len [int] Length of the data array.														   *
 * @return void																						   *
 * 																									   *
 * @note The calculate_average function must be run first, as the average is used in the calculation   *
 * 		 of the standard deviation.																	   *
 ******************************************************************************************************/

void calculate_sd(float data[], int len)
{
	for(int i = 0; i < len; i++) {
		standard_dev += ( (data[i]-average) * (data[i]-average) );
	}
	standard_dev = sqrt(standard_dev / len);
}

/*******************************************************************************************************
 * @brief Splits a float into integer and decimal parts.											   *
 * 																									   *
 * This function separates a floating-point number into its integer and decimal parts based on the 	   *
 * specified number of decimal places. The integer and decimal parts are stored in the provided 	   *
 * integer pointers.																				   *
 * 																									   *
 * @param int_val [int*] Pointer to store the integer part of the float.							   *
 * @param dec_val [int*] Pointer to store the decimal part of the float.							   *
 * @param float_val [float] Floating-point value to split.											   *
 * @param dec_places [int] Number of decimal places to consider.									   *
 * @return void																						   *
 ******************************************************************************************************/

void split_float_into_ints(int *int_val, int *dec_val, float float_val, int dec_places)
{
	int tens = pow(10, dec_places);
	*int_val = (int)float_val;
	*dec_val = (int)((float_val * tens) - (*int_val * tens));
}

/*******************************************************************************************************
 * @brief Sets the PWM duty cycle for a specified timer channel.									   *
 * 																									   *
 * This function sets the PWM duty cycle by calculating the appropriate compare value based on the 	   *
 * specified duty cycle percentage and the timer's auto-reload value (period).						   *
 * 																									   *
 * @param htim [TIM_HandleTypeDef*] Handle to the timer.											   *
 * @param channel [uint32_t] Timer channel to set the duty cycle for.								   *
 * @param duty_cycle_percent [uint8_t] Duty cycle percentage to set (0-100).						   *
 * @return void																						   *
 * 																									   *
 * @note This function assumes the timer and channel have already been configured for PWM mode.		   *
 ******************************************************************************************************/

void set_pwm_duty_cycle(TIM_HandleTypeDef *htim, uint32_t channel, uint8_t duty_cycle_percent)
{
	// Get timer auto-reload value (i.e. period)
	uint32_t timer_period = __HAL_TIM_GET_AUTORELOAD(htim);

	// Calculate the proper compare value to be loaded into the capture/compare register (CCR)
	uint32_t compare_value = (duty_cycle_percent * timer_period) / 100;

	// Set new duty cycle
    __HAL_TIM_SET_COMPARE(htim, channel, compare_value);
}

/*******************************************************************************************************
 * @brief PID controller for motor speed control.													   *
 * 																									   *
 * This function implements a PID controller to calculate the new duty cycle for the motor based on    *
 * the setpoint and measured value. The PID gains (Kp, Ki, Kd) are used to compute the error, 		   *
 * integral, and derivative terms.																	   *
 * 																									   *
 * @param setpoint [float] Desired motor speed.														   *
 * @param measured_value [float] Current motor speed.												   *
 * @return float New duty cycle percentage (0-100).													   *
 * 																									   *
 * @note This function only operates if the motor algorithm is set to PID control (motor_algo == 1).   *
 * @note The function ensures that the duty cycle remains within the range of 0 to 100.				   *
 ******************************************************************************************************/

float pid_controller(float setpoint, float measured_value)
{
    if(motor_algo == 1) {
		float error = setpoint - measured_value;
		integral += error * dt;
		float derivative = (error - last_error) / dt;
		last_error = error;
		float output = Kp * error + Ki * integral + Kd * derivative;

		// Calculate new duty cycle
		duty_cycle += output;

		// Ensure the duty cycle is within the range of 0 to 100 (as duty cycle percentage)
		if (duty_cycle > 100.0f) duty_cycle = 100.0f;
		if (duty_cycle < 0.0f) duty_cycle = 0.0f;

		return duty_cycle;
    }
    else {
    	return duty_cycle;
    }
}

/*******************************************************************************************************
 * @brief Checks if a string represents a numeric value.											   *
 * 																									   *
 * This function checks if the provided string contains only numeric characters (0-9) or a single 	   *
 * decimal point, indicating that it represents a valid numeric value.								   *
 * 																									   *
 * @param str [const char*] Input string to check.													   *
 * @return int 1 if the string is numeric, 0 otherwise.												   *
 * 																									   *
 * @note An empty string is considered not numeric.													   *
 ******************************************************************************************************/

int isNumeric(const char *str) {
    int hasDecimalPoint = 0;

    // Check for empty string
    if (*str == '\0') {
        return 0;
    }

    // Check each character in the string
    while (*str) {
        if (!isdigit((unsigned char)*str)) {  // Cast to unsigned char
            // Allow one decimal point
            if (*str == '.' && !hasDecimalPoint) {
                hasDecimalPoint = 1;
            } else {
                return 0; // Not a digit or second decimal point
            }
        }
        str++;
    }
    return 1; // All characters are digits or one decimal point
}

/*******************************************************************************************************
 * @brief Checks the parameter string for validity and sets motor parameter if valid value.			   *
 * 																									   *
 * This function checks for valid input, then updates the specified PID parameter (Kp, Kd, or Ki) 	   *
 * accordingly by converting the latter portion of the message into a float value.					   *
 * 																									   *
 * @param msg [message_t*] A pointer to the message structure containing the payload.				   *
 * @return int Pass (1) or fail (0).																   *
 ******************************************************************************************************/

int parse_param_string(message_t *msg)
{
    // Check if the input string is 7 characters long (parameter name and float value)
    int len = strlen((char *)msg->payload);
    if (len != 7) return 0;

    // Check if the first character is 'K'
    if (msg->payload[0] != 'K') return 0;

    // Check if the remaining characters form a float per the template
    if(!isdigit(msg->payload[2])) return 0;
    if(msg->payload[3] != '.') return 0;
    if(!isdigit(msg->payload[4])) return 0;
    if(!isdigit(msg->payload[5])) return 0;
    if(!isdigit(msg->payload[6])) return 0;

    // Determine which PID parameter to change
    const uint8_t *ptr = &msg->payload[2];
    if(msg->payload[1] == 'p') {			// Kp
    	Kp = atof((const char *)ptr);
    	return 1;
    }
    else if(msg->payload[1] == 'd') {		// Kd
    	Kd = atof((const char *)ptr);
    	return 1;
    }
    else if(msg->payload[1] == 'i') {		// Ki
		Ki = atof((const char *)ptr);
		return 1;
	}

    return 0;
}
