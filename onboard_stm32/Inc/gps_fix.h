
// gps_fix.h file
#ifndef _gps_fix_h
#define _gps_fix_h

typedef struct _gpsFix {
	int fixTime;
	char * latitude;
	char * longitude;
	int fixQuality;
	int numSatellites;
	char * altitude;
} GPSFIX;

GPSFIX GPSFIX_Const(int fixTime, char * latitude, char * longitude,	int fixQuality, int numSatellites, char * altitude);
void GPSFIX_Dest(GPSFIX gpsFix);
void setGpsFixFromGps(GPSFIX * gpsFix);

#endif
