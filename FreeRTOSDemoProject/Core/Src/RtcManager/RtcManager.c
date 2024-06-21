
/*=====================================================================================*\
| Author:   Christopher Coyne                                          June 20th, 2024  |
| --------------------------------------------------------------------------------------|
| Date:     June 20th, 2024                                                             |
| --------------------------------------------------------------------------------------|
| MODULE:     [ RtcManager ]                                                            |
| FILE:       RtcManager.c                                                              |
| --------------------------------------------------------------------------------------|
| DESCRIPTION:                                                                          |
|    The `RtcManager` module is responsible for handling user input and configuration   |
|    of the real time clock (RTC).                                                      |
\*=====================================================================================*/

/****************************************************
 *  Include files                                   *
 ****************************************************/

#include "FreeRTOS.h"
#include "main.h"
#include "task.h"
#include "queue.h"
#include "RtcManager.h"
#include "Config_RtcManager.h"
#include "UartManager.h"
#include <string.h>
#include <stdio.h>

/****************************************************
 *  Function prototypes                             *
 ****************************************************/

uint8_t getnumber(uint8_t *p, int len);
int validate_rtc_information(RTC_TimeTypeDef *time, RTC_DateTypeDef *date);
void rtc_configure_time(RTC_TimeTypeDef *time);
void rtc_configure_date(RTC_DateTypeDef *date);
void show_time_date(void);

/****************************************************
 *  Messages                                        *
 ****************************************************/

const char *msg_inv_rtc = "\n***** Invalid RTC option ******\n";
const char *msg_conf = "\nRTC configuration successful\n";

const char *msg_rtc_hh = "\nEnter hour (1-12): ";
const char *msg_rtc_mm = "Enter minutes (0-59): ";
const char *msg_rtc_ss = "Enter seconds (0-59): ";
const char *msg_rtc_ampm = "Enter (0) AM or (1) PM: ";

const char *msg_rtc_mo = "\nEnter month (1-12): ";
const char *msg_rtc_dd = "Enter date (1-31): ";
const char *msg_rtc_yr = "Enter year (0-99): ";
const char *msg_rtc_dow = "Enter day of the week (1-7, Sun=1): ";

const char *msg_rtc_menu_1 = "\n======================================\n"
				  		       "|               RTC Menu             |\n"
						       "======================================\n";
const char *msg_rtc_menu_2 = "\n Date ---> Configure date\n"
							 " Time ---> Configure time\n"
							 " Rfsh ---> Refresh the time and date\n"
							 " Main ---> Return to main menu\n\n"
							 " Enter your selection here: ";

/****************************************************
 *  Variables                                       *
 ****************************************************/

static int curr_rtc_state = HH_CONFIG;
RTC_TimeTypeDef time;
RTC_DateTypeDef date;

/****************************************************
 *  Public functions                                *
 ****************************************************/

void rtc_task(void *param)
{
	uint32_t msg_addr;
	message_t *msg;

	while(1) {

		// Wait for notification from another task
		xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);

		while(curr_sys_state != sMainMenu) {

			switch(curr_sys_state) {
				case sRtcMenu:
					// Display RTC menu for the user
					xQueueSend(q_print, &msg_rtc_menu_1, portMAX_DELAY);
					show_time_date();
					xQueueSend(q_print, &msg_rtc_menu_2, portMAX_DELAY);

					// Wait for the user to select their desired RTC configuration option
					xTaskNotifyWait(0, 0, &msg_addr, portMAX_DELAY);
					msg = (message_t*)msg_addr;

					// Process command, update date / time accordingly
					if(msg->len <= 4) {
						if(!strcmp((char*)msg->payload, "Date")) {			// Configure date
							curr_sys_state = sRtcDateConfig;
							xQueueSend(q_print, &msg_rtc_mo, portMAX_DELAY);
						}
						else if (!strcmp((char*)msg->payload, "Time")) {	// Configure time
							curr_sys_state = sRtcTimeConfig;
							xQueueSend(q_print, &msg_rtc_hh, portMAX_DELAY);
						}
						else if (!strcmp((char*)msg->payload, "Rfsh")) {	// Refresh the date and time
							curr_sys_state = sRtcMenu;
						}
						else if (!strcmp((char*)msg->payload, "Main")) {	// Back to main menu
							// Update the system state
							curr_sys_state = sMainMenu;
						}
						else {												// Invalid response
							// Update the system state
							curr_sys_state = sMainMenu;
							xQueueSend(q_print, &msg_inv_rtc, portMAX_DELAY);
						}
					}
					else {
						// If user input is longer than 4 characters, notify user of invalid response
						curr_sys_state = sMainMenu;
						xQueueSend(q_print, &msg_inv_rtc, portMAX_DELAY);
					}
					break;
				case sRtcDateConfig:
					// Wait for the user to select their desired RTC configuration option
					xTaskNotifyWait(0, 0, &msg_addr, portMAX_DELAY);
					msg = (message_t*)msg_addr;

					// Configure month, date, year, or day of week accordingly
					switch(curr_rtc_state) {
						case MONTH_CONFIG:
							uint8_t m = getnumber(msg->payload, msg->len);
							date.Month = m;
							curr_rtc_state = DATE_CONFIG;
							xQueueSend(q_print, &msg_rtc_dd, portMAX_DELAY);
							break;
						case DATE_CONFIG:
							uint8_t d = getnumber(msg->payload, msg->len);
							date.Date = d;
							curr_rtc_state = YEAR_CONFIG;
							xQueueSend(q_print, &msg_rtc_yr, portMAX_DELAY);
							break;
						case YEAR_CONFIG:
							uint8_t y = getnumber(msg->payload, msg->len);
							date.Year = y;
							curr_rtc_state = DAY_CONFIG;
							xQueueSend(q_print, &msg_rtc_dow, portMAX_DELAY);
							break;
						case DAY_CONFIG:
							uint8_t day = getnumber(msg->payload, msg->len);
							date.WeekDay = day;

							if(!validate_rtc_information(NULL, &date)) {
								rtc_configure_date(&date);
								xQueueSend(q_print, &msg_conf, portMAX_DELAY);
							}
							else {
								xQueueSend(q_print, &msg_inv_rtc, portMAX_DELAY);
							}

							// Update system state
							curr_sys_state = sRtcMenu;
							curr_rtc_state = 0;
							break;
					}
					break;
				case sRtcTimeConfig:
					// Wait for the user to select their desired RTC configuration option
					xTaskNotifyWait(0, 0, &msg_addr, portMAX_DELAY);
					msg = (message_t*)msg_addr;

					// Configure hours, minutes, or seconds accordingly
					switch(curr_rtc_state) {
						case HH_CONFIG:
							uint8_t hour = getnumber(msg->payload, msg->len);
							time.Hours = hour;
							curr_rtc_state = MM_CONFIG;
							xQueueSend(q_print, &msg_rtc_mm, portMAX_DELAY);
							break;
						case MM_CONFIG:
							uint8_t min = getnumber(msg->payload, msg->len);
							time.Minutes = min;
							curr_rtc_state = SS_CONFIG;
							xQueueSend(q_print, &msg_rtc_ss, portMAX_DELAY);
							break;
						case SS_CONFIG:
							uint8_t sec = getnumber(msg->payload, msg->len);
							time.Seconds = sec;
							curr_rtc_state = AMPM_CONFIG;
							xQueueSend(q_print, &msg_rtc_ampm, portMAX_DELAY);
							break;
						case AMPM_CONFIG:
							uint8_t opt = getnumber(msg->payload, msg->len);
							time.TimeFormat = opt; // Note: 0 = RTC_HOURFORMAT12_AM, 1 = RTC_HOURFORMAT12_PM
							if(!validate_rtc_information(&time, NULL)) {
								rtc_configure_time(&time);
								xQueueSend(q_print, &msg_conf, portMAX_DELAY);
							}
							else {
								xQueueSend(q_print, &msg_inv_rtc, portMAX_DELAY);
							}
							// Update system state
							curr_sys_state = sRtcMenu;
							curr_rtc_state = 0;
							break;
					}
					break;
				default:
					curr_sys_state = sMainMenu;
					xQueueSend(q_print, &msg_inv_rtc, portMAX_DELAY);
					break;
			}

		} // while end

		// Notify the main menu task
		xTaskNotify(handle_main_menu_task, 0, eNoAction);

	} // while super loop end
}

/****************************************************
 *  Private functions                               *
 ****************************************************/

uint8_t getnumber(uint8_t *p, int len)
{
	return (len > 1) ? ( ((p[0]-48) * 10) + (p[1]-48) ) : (p[0]-48);
}

int validate_rtc_information(RTC_TimeTypeDef *time, RTC_DateTypeDef *date)
{
	if(time) {
		if( (time->Hours > 12) || (time->Minutes > 59) || (time->Seconds > 59) || (time->TimeFormat > 1) )
			return 1;
	}
	if(date) {
		if( (date->Date > 31) || (date->WeekDay > 7) || (date->Year > 99) || (date->Month > 12) )
			return 1;
	}

	return 0;
}

void rtc_configure_time(RTC_TimeTypeDef *time)
{
	time->DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	time->StoreOperation = RTC_STOREOPERATION_RESET;

	HAL_RTC_SetTime(&hrtc, time, RTC_FORMAT_BIN);
}

void rtc_configure_date(RTC_DateTypeDef *date)
{
	HAL_RTC_SetDate(&hrtc, date, RTC_FORMAT_BIN);
}

void show_time_date(void)
{
	static char showtime[40];
	static char showdate[40];
	char* weekday;

	RTC_DateTypeDef rtc_date;
	RTC_TimeTypeDef rtc_time;

	static char *time = showtime;
	static char *date = showdate;

	memset(&rtc_date, 0, sizeof(rtc_date));
	memset(&rtc_time, 0, sizeof(rtc_time));

	// Get the RTC current time
	HAL_RTC_GetTime(&hrtc, &rtc_time, RTC_FORMAT_BIN);
	// Get the RTC current date
	HAL_RTC_GetDate(&hrtc, &rtc_date, RTC_FORMAT_BIN);

	char *format;
	format = (rtc_time.TimeFormat == RTC_HOURFORMAT12_AM) ? "AM" : "PM";

	// Display time format: hh:mm:ss [AM/PM]
	sprintf((char*)showtime, "%s:\t%02d:%02d:%02d [%s]", "\nCurrent Time & Date", rtc_time.Hours, rtc_time.Minutes, rtc_time.Seconds, format);
	xQueueSend(q_print, &time, portMAX_DELAY);

	// Display date format: day, month-date-year
	switch(rtc_date.WeekDay) {
		case 1:
			weekday = "Sunday";
			break;
		case 2:
			weekday = "Monday";
			break;
		case 3:
			weekday = "Tuesday";
			break;
		case 4:
			weekday = "Wednesday";
			break;
		case 5:
			weekday = "Thursday";
			break;
		case 6:
			weekday = "Friday";
			break;
		case 7:
			weekday = "Saturday";
			break;
	}
	sprintf((char*)showdate, "\t%s, %02d-%02d-%02d\n", weekday, rtc_date.Month, rtc_date.Date, rtc_date.Year + 2000);
	xQueueSend(q_print, &date, portMAX_DELAY);
}
