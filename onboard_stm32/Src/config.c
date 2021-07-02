
// config.c file
#include <string.h>
#include <stdlib.h>
#include "stm32f4xx_hal.h"
#include "config.h"
#include "rtc_date_time.h"

extern RTCDATETIME rtcDateTime;

// Constructor for Config object
CONFIG CONFIG_Const(
		uint8_t start_hours, 
		uint8_t start_minutes, 
		uint8_t end_hours, 
		uint8_t end_minutes,
		uint32_t stby_log_interval,
		int umweltspaeher_id,
		uint32_t adc_mcp3903_config_delay, // Delay each time the ADC gets the continuous readout config
		uint32_t loops_for_telemetry,
		uint64_t loops_for_new_file,
		uint32_t gps_waiting_time){
  CONFIG* config = malloc(sizeof(CONFIG));
	config->start_hours = start_hours;
  config->start_minutes = start_minutes;
  config->end_hours = end_hours;
	config->end_minutes = end_minutes;
	config->stby_log_interval = stby_log_interval;
	config->umweltspaeher_id = umweltspaeher_id;
	config->adc_mcp3903_config_delay = adc_mcp3903_config_delay;
	config->loops_for_telemetry = loops_for_telemetry;
	config->loops_for_new_file = loops_for_new_file;
	config->gps_waiting_time = gps_waiting_time;
  return *config;
}

// Destructor for CONFIG structure
void CONFIG_Dest(CONFIG config) {
		free(&config);
}

// Will return 1 if the current time is within the timeframe
int startRecording(CONFIG config) {
	float hours_current_time = (float)bcd_to_decimal(rtcDateTime.sTime.Hours) + ((float)bcd_to_decimal(rtcDateTime.sTime.Minutes))/60.0; //  + (float)rtcDateTime.sTime.Seconds/(60.0*60.0)
	float hours_start_time = (float)config.start_hours + ((float)config.start_minutes)/60.0;
	float hours_end_time = (float)config.end_hours + ((float)config.end_minutes)/60.0;
	
	if (hours_current_time >= hours_start_time && //rtcDateTime.sTime.Minutes >= config.start_minutes &&
			hours_current_time <= hours_end_time //&& rtcDateTime.sTime.Minutes <= config.end_minutes
			) {
		return 1;
	}
	else {
		return 0;
	}
}

// 
uint64_t calcTotalLoops(CONFIG config) {
	float hours_current_time = (float)bcd_to_decimal(rtcDateTime.sTime.Hours) + ((float)bcd_to_decimal(rtcDateTime.sTime.Minutes))/60.0; //  + (float)rtcDateTime.sTime.Seconds/(60.0*60.0)
	float hours_end_time = (float)config.end_hours + ((float)config.end_minutes)/60.0;
	
	// For debugging
	char strHours[50];
	sprintf(strHours, "rtcDateTime.sTime.Hours: %u   hours_current_time: %f   hours_end_time: %f\r\n", bcd_to_decimal(rtcDateTime.sTime.Hours), hours_current_time, hours_end_time);
	print_uart2(strHours);
	 
	// total time in 0.2 microseconds (loops)
	uint64_t totalLoops = (unsigned int)((hours_end_time - hours_current_time)* 60 * 60 * 5000);
	// return the total number of loops that need to be run
	return totalLoops;
}
