
// _rtc_date_time.h file
#ifndef _rtc_date_time_h
#define _rtc_date_time_h

typedef struct _rtcDateTime {
	int isSet;
	RTC_DateTypeDef sDate;
	RTC_TimeTypeDef sTime;
	RTC_AlarmTypeDef sAlarm;
} RTCDATETIME;

RTCDATETIME RTCDATETIME_Const(int isSet);
void RTCDATETIME_Dest(RTCDATETIME rtcDateTime);
void setRtcDateTimeFromGps(RTCDATETIME * rtcDateTime);

#endif
