
//utility_umweltspaeher.h file
#ifndef _utility_umweltspaeher_h
#define _utility_umweltspaeher_h

#define VREFINT_CAL_ADDR    0x1FFFF7BA;
 
/* prints a message to screen for testing */
void print_uart2(char* strToPrint);
/* prints a message to screen for testing */

void print(UART_HandleTypeDef huart, char* strToPrint);

void println(UART_HandleTypeDef huart, char* strToPrint);

//void substring(int start, int finish, char *string_to_use);

int split (char *str, char c, char ***arr);

char * concatenate(const char *a, const char *b, const char *c);

uint32_t DWT_Delay_Init(void);

void DWT_Delay_us(volatile uint32_t microseconds);

void DWT_Delay_us_test(void);

void byte_to_hexString(char* strToReturn, uint8_t byteToConvert);
	
void byte_array_to_hexString(char* strToReturn, uint8_t* byteToConvert,  uint32_t len);

void floatToByteArray(uint8_t* byte_array, int start_pos, float float_to_convert);

void print_RTC_time(RTC_TimeTypeDef sTime);
void print_RTC_date(RTC_DateTypeDef sDate);
uint8_t bcd_to_decimal(uint8_t x);
uint8_t decToBcd(uint8_t val);
void get_Rtc_date_time(RTC_HandleTypeDef* hrtc, RTC_TimeTypeDef sTime, RTC_DateTypeDef sDate, char* rtc_date_time);

void get_Rtc_date_time_for_raspi_sync(RTC_HandleTypeDef* hrtc, RTC_TimeTypeDef sTime, RTC_DateTypeDef sDate, char* rtc_date_time);
void print_free_memory(void);

float get_internal_temp_from_internal_adc(ADC_HandleTypeDef hadc);
void get_Rtc_date(RTC_HandleTypeDef* hrtc, RTC_DateTypeDef sDate, char* rtc_date);

void printLCDStatic(char* strToPrint, int delay);

void substring(char *dest, char *src, int start, int finish);
void substring_micro(char *dest, char *src, int start, int finish);

#endif
