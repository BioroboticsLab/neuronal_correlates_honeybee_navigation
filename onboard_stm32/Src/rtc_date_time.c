#include <stdlib.h>
#include <string.h>
#include "stm32f4xx_hal.h"
#include "rtc_date_time.h"
#include "utility_umweltspaeher.h"

// Global UART and rtc devices
// UART1 is asigned to GPS
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern RTC_HandleTypeDef hrtc;

// Constructor for RTCDATETIME
RTCDATETIME RTCDATETIME_Const(int isSet) {
	RTCDATETIME * rtcDateTime = malloc(sizeof(RTCDATETIME));
	rtcDateTime->isSet = isSet;
	return *rtcDateTime;
}

// Destructor for RTCDATETIME
void RTCDATETIME_Dest(RTCDATETIME rtcDateTime) {
	free(&rtcDateTime);
}

// Set the date (day, month, year) for RTC and time (hours, minutes, seconds, subseconds)
// If a GPS signal is found then a 1 is returned, otherwise 0
void setRtcDateTimeFromGps(RTCDATETIME * rtcDateTime) {	
	// GPS is assumed to not be working
	(*rtcDateTime).isSet = 0;
	// Read from GPS via UART1 into buffer
	char buffer[1200] = "\0";
	HAL_UART_Receive(&huart1, (uint8_t*) buffer, 1200, 200); // For old gps use: HAL_UART_Receive(&huart1, (uint8_t*) buffer, 1200, 2000);
		// PRINT GPS STRING RECEIVED VIA UART
	println(huart2, "GPS STRING for Time: ");
	println(huart2, buffer); 
	// loop through all lines
	char * lineToken = strtok(buffer, "$");
	while (lineToken != NULL) {
		// find line starting with GPRMC
		if (strncmp(lineToken, "GPRMC", strlen("GPRMC")) == 0) {
			print(huart2, "lineToken: ");
			println(huart2, lineToken);
			char **values = NULL;
			char temp[2];
			// Split string using comma delimeter
			int count = split(lineToken, ',', &values);
			// Determine if the first chars are '\0' and hence empty
			// if they are empty, then skip the reading
			if (values[1][0] != '\0' || values[9][0] != '\0') {
				// Set the date values
				sprintf(temp, "%.*s\n", 2 - 0, &values[9][0]);
				(*rtcDateTime).sDate.Date = decToBcd((uint8_t)atol(temp));
				sprintf(temp, "%.*s\n", 4 - 2, &values[9][2]);
				(*rtcDateTime).sDate.Month = decToBcd((uint8_t)atol(temp));
				sprintf(temp, "%.*s\n", 6 - 4, &values[9][4]);
				(*rtcDateTime).sDate.Year = decToBcd((uint8_t)atol(temp));
				// attempt to set the date for the rtc
				if (HAL_RTC_SetDate(&hrtc, &(*rtcDateTime).sDate, RTC_FORMAT_BCD) != HAL_OK) {
					println(huart2, "HAL_RTC_SetDate Failed");
				}
				// Set the time values
				sprintf(temp, "%.*s\n", 2 - 0, &values[1][0]);
				//print(huart2, "  hh=");
				//print(huart2, temp);
				(*rtcDateTime).sTime.Hours = decToBcd((uint8_t)atol(temp));
				sprintf(temp, "%.*s\n", 4 - 2, &values[1][2]);
				//print(huart2, "  mm=");
				//print(huart2, temp);
				(*rtcDateTime).sTime.Minutes = decToBcd((uint8_t)atol(temp));
				sprintf(temp, "%.*s\n", 6 - 4, &values[1][4]);
				//print(huart2, "   ss=");
				//print(huart2, temp);
				(*rtcDateTime).sTime.Seconds = decToBcd((uint8_t)atol(temp));
				sprintf(temp, "%.*s\n", 9 - 7, &values[1][7]);
				//print(huart2, "   ms=");
				//print(huart2, temp);
				(*rtcDateTime).sTime.SubSeconds = decToBcd((uint8_t)atol(temp));
				// Indicate that the time has been set
				(*rtcDateTime).isSet = 1;
				// attempt to set the time for the rtc
				if (HAL_RTC_SetTime(&hrtc, &(*rtcDateTime).sTime, RTC_FORMAT_BCD) != HAL_OK) {
					println(huart2, "HAL_RTC_SetTime Failed");
				}
				// found GPRMC line so no need to loop through line tokens anymore
				/* freeing values */
				for (int i = 0; i < count; i++) free (values[i]);
				free(values);
				break;
			}
		}
		lineToken = strtok(NULL, "$");
	}
}

	
	char * str[200];
	//println(huart2, buffer);
	
	
