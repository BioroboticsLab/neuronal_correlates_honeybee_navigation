
// config.h file
#ifndef __CONFIG_H
#define __CONFIG_H

#include <time.h>
 
typedef struct _config {
	uint8_t start_hours;
	uint8_t start_minutes;
	uint8_t end_hours;
	uint8_t end_minutes;
	uint32_t stby_log_interval;
  uint8_t total_loops;
	int umweltspaeher_id;
	uint32_t adc_mcp3903_config_delay;
	uint32_t loops_for_telemetry;
	uint64_t loops_for_new_file;
	uint32_t gps_waiting_time;
} CONFIG;

CONFIG CONFIG_Const(
		uint8_t start_hours, 
		uint8_t start_minutes, 
		uint8_t end_hours, 
		uint8_t end_minutes,
		uint32_t stby_log_interval,
		int umweltspaeher_id,
		uint32_t adc_mcp3903_config_delay,
		uint32_t loops_for_telemetry,
		uint64_t loops_for_new_file,
		uint32_t gps_waiting_time
);
void CONFIG_Dest(CONFIG config);
int startRecording(CONFIG config);
uint64_t calcTotalLoops(CONFIG config);

#endif
