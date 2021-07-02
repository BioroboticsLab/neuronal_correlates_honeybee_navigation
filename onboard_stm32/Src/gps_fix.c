#include <stdlib.h>
#include <string.h>
#include "stm32f4xx_hal.h"
#include "gps_fix.h"
#include "utility_umweltspaeher.h"

// Global UART and rtc devices
// UART1 is asigned to GPS
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern RTC_HandleTypeDef hrtc;

// Constructor for GPSFIX
GPSFIX GPSFIX_Const(int fixTime, char * latitude, char * longitude,	int fixQuality, int numSatellites, char * altitude) { 
  GPSFIX * gpsFix = malloc(sizeof(GPSFIX));
  gpsFix->fixTime = fixTime;
  gpsFix->latitude = latitude;
	gpsFix->longitude = longitude;
	gpsFix->fixQuality = fixQuality;
	gpsFix->numSatellites = numSatellites;
	gpsFix->altitude = altitude;
  return *gpsFix;
}

// Destructor for GPSFIX
void GPSFIX_Dest(GPSFIX gpsFix) {
	free(&gpsFix);
}

void setGpsFixFromGps(GPSFIX * gpsFix) {
	// Read from GPS via UART1 into buffer
	char buffer[1200] = "\0";
	HAL_UART_Receive(&huart1, (uint8_t*) buffer, 1200, 2000);
	// loop through all lines
	char * lineToken = strtok(buffer, "$");
	
	// PRINT GPS STRING RECEIVED VIA UART
	println(huart2, "GPS STRING: ");
	println(huart2, buffer); 
	println(huart2, lineToken);
	
	while (lineToken != NULL) {
		// find line starting with GPGGA
		if (strncmp(lineToken, "GPGGA", strlen("GPGGA")) == 0) {
			// next line only for testing
			//lineToken = "GPGGA,100137.00,5227.42504,N,01317.81601,E,1,06,1.14,74.6,M,42.3,M,,*65";
			char **values = NULL;
			int count = split(lineToken, ',', &values);
			// Determine if the first chars are '\0' and hence empty
			// if they are empty, then skip the reading
			if (values[1][0] != '\0' && values[2][0] != '\0' && values[3][0] != '\0' &&
					values[4][0] != '\0' && values[5][0] != '\0' && values[6][0] != '\0' &&
					values[7][0] != '\0' && values[9][0] != '\0' && values[10][0] != '\0') {
				// set the fixTime, only the first 6 characters are necessary
				char temp[6];
				sprintf(temp, "%.*s\n", 6 - 0, &values[1][0]);
				(*gpsFix).fixTime = atoi(temp);
				// set the latitude
				(*gpsFix).latitude = concatenate(values[2], " ", values[3]);
				// set the longitude
				(*gpsFix).longitude = concatenate(values[4], " ", values[5]);
				// set the fixQuality
				(*gpsFix).fixQuality = atoi(values[6]);
				// set the number of satillites
				(*gpsFix).numSatellites = atoi(values[7]);
				// set the altitude
				(*gpsFix).altitude = concatenate(values[9], " ", values[10]);
				// found GPGGA line so no need to loop through line tokens anymore
				break;
			}
		/* freeing tokens */
		for (int i = 0; i < count; i++) free (values[i]);
		free(values);
		}
		lineToken = strtok(NULL, "$");
	}
}
