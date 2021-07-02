
// utility_umweltspaeher.c file
#include <string.h>
#include <stdlib.h>
#include "stm32f4xx_hal.h"
#include "utility_umweltspaeher.h"
#include "display_i2c.h"

extern UART_HandleTypeDef huart2;

/** Prints a given char* on USART */
void print_uart2(char* strToPrint){
	HAL_UART_Transmit(&huart2, (uint8_t *) strToPrint, strlen(strToPrint), 50);
}

/** Prints a given char* on USART */
void print(UART_HandleTypeDef huart, char* strToPrint){
	HAL_UART_Transmit(&huart, (uint8_t *) strToPrint, strlen(strToPrint), 50);
}

void println(UART_HandleTypeDef huart, char* strToPrint){
	print(huart, strToPrint);
	HAL_UART_Transmit(&huart, (uint8_t *) "\r\n", 2, 50);
}

void printLCDStatic(char* strToPrint, int delay) {
		if (strToPrint != NULL) {
				int length = strlen(strToPrint);
				LCM1602_Display_ClearDisplay();
				LCM1602_Display_ReturnHome();
				if (length > LC_DISPLAY_COL) {
						char firstLine[17], secondLine[length - 16];
						substring_micro(firstLine, strToPrint, 0, 16);
						firstLine[16] = '\0';
					  LCM1602_Display_WriteString(firstLine);
					  LCM1602_Display_SetCursorPosition(2, 9);
						int start = 16;
					  while(1) {
							if (strToPrint[start] == ' ') start++;
							else break;
						}
						substring_micro(secondLine, strToPrint, start, length);
						secondLine[length - start] = '\0';
						LCM1602_Display_WriteString(secondLine);
				}
				else {
					LCM1602_Display_WriteString(strToPrint);
				}
				LCM1602_Display_Delay(LC_PWRUP_DELAY * delay);
		}
}

void substring(char *dest, char *src, int start, int finish) {
	sprintf(dest, "%.*s\n", finish - start, &src[start]);
}

// Substring without sprintf which does have/cause a memory leak
void substring_micro(char *dest, char *src, int start, int finish) {
	for (int i=start; i<finish; i++){
		dest[i-start] = src[i];
		dest[i+1] = 10;
	}
}


int split(char *txt, char delim, char ***tokens)
{
    int *tklen, *t, count = 1;
    char **arr, *p = (char *) txt;

    while (*p != '\0') if (*p++ == delim) count += 1;
    t = tklen = calloc (count, sizeof (int));
    for (p = (char *) txt; *p != '\0'; p++) *p == delim ? *t++ : (*t)++;
    *tokens = arr = malloc (count * sizeof (char *));
    t = tklen;
    p = *arr++ = calloc (*(t++) + 1, sizeof (char *));
    while (*txt != '\0')
    {
        if (*txt == delim)
        {
            p = *arr++ = calloc (*(t++) + 1, sizeof (char *));
            txt++;
        }
        else *p++ = *txt++;
    }
    free (tklen);
    return count;
}

void print_free_memory(void){
	// perform free memory check
	int blockSize = 16;
	int i = 1;
	char temp_str[200];
	//sprintf(temp_str, "Checking memory with blocksize %d char ...\n", blockSize);
	//println(huart2, temp_str);
	while (1) {
			char *p = (char *) malloc(i * blockSize);
			if (p == NULL)
					break;
			free(p);
			++i;
	}
	sprintf(temp_str, "MEMORY AVAILABLE %d char\n", (i - 1) * blockSize);
	println(huart2, temp_str);
}

/*
// splits a string and keeps empty tokens
int split(char *str, char c, char ***arr) {
	int count = 1;
	int token_len = 1;
	int i = 0;
	char *p;
	char *t;

	p = str;
	while (*p != '\0') {
		if (*p == c) {
			count++;
		}
		p++;
	}

	*arr = (char**) malloc(sizeof(char*) * count);
	if (*arr == NULL) {
		//exit(1);
		return 0;
	}

	p = str;
	while (*p != '\0') {
		if (*p == c) {
				(*arr)[i] = (char*) malloc( sizeof(char) * token_len );
				if ((*arr)[i] == NULL) {
					//exit(1);
					return 0;
				}
				token_len = 0;
				i++;
		}
		p++;
		token_len++;
	}
	(*arr)[i] = (char*) malloc( sizeof(char) * token_len );
	if ((*arr)[i] == NULL) {
		//exit(1);
		return 0;
	}

	i = 0;
	p = str;
	t = ((*arr)[i]);
	while (*p != '\0') {
		if (*p != c && *p != '\0') {
			*t = *p;
			t++;
		}
		else {
			*t = '\0';
			i++;
			t = ((*arr)[i]);
		}
		p++;
	}
	return count;
} */

char * concatenate(const char *a, const char *b, const char *c) {
    size_t alen = strlen(a);
    size_t blen = strlen(b);
    size_t clen = strlen(c);
    char *res = malloc(alen + blen + clen + 1);
    memcpy(res, a, alen);
    memcpy(res + alen, b, blen);
    memcpy(res + alen + blen, c, clen + 1);
    return res;
}

/**
 * @brief  Initializes DWT_Clock_Cycle_Count for DWT_Delay_us function
 * @return Error DWT counter
 *         1: clock cycle counter not started
 *         0: clock cycle counter works
 */

// Code from: https://community.st.com/thread/13838
uint32_t DWT_Delay_Init(void) {
  /* Disable TRC */
  CoreDebug->DEMCR &= ~CoreDebug_DEMCR_TRCENA_Msk; // ~0x01000000;
  /* Enable TRC */
  CoreDebug->DEMCR |=  CoreDebug_DEMCR_TRCENA_Msk; // 0x01000000;
     
  /* Disable clock cycle counter */
  DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk; //~0x00000001;
  /* Enable  clock cycle counter */
  DWT->CTRL |=  DWT_CTRL_CYCCNTENA_Msk; //0x00000001;
     
  /* Reset the clock cycle counter value */
  DWT->CYCCNT = 0;
     
     /* 3 NO OPERATION instructions */
     __ASM volatile ("NOP");
     __ASM volatile ("NOP");
  __ASM volatile ("NOP");

  /* Check if clock cycle counter has started */
     if(DWT->CYCCNT)
     {
       return 0; /*clock cycle counter started*/
     }
     else
  {
    return 1; /*clock cycle counter not started*/
  }
}

void DWT_Delay_us(volatile uint32_t microseconds)
{
  uint32_t clk_cycle_start = DWT->CYCCNT;

  /* Go to number of cycles for system */
  microseconds *= (HAL_RCC_GetHCLKFreq() / 1000000);

  /* Delay till end */
  while ((DWT->CYCCNT - clk_cycle_start) < microseconds);
}

void DWT_Delay_us_test(void){
	DWT_Delay_Init();
	println(huart2, "DELAY TEST");
	DWT_Delay_us(1000000);
	println(huart2, "1s DELAY TEST");
	DWT_Delay_us(2000000);
	println(huart2, "2s DELAY TEST");
	DWT_Delay_us(4000000);
	println(huart2, "4s DELAY TEST");
	DWT_Delay_us(1000000);
	println(huart2, "1s DELAY TEST");
}
// Writes a String representing a hex of the given byte testByte 
/*Example usage:
  uint8_t testByte = 122;
	char str_to_print_byte[10];
	byte_to_hexString(str_to_print_byte, testByte);
	println(huart2, str_to_print_byte);*/
void byte_to_hexString(char* strToReturn, uint8_t byteToConvert){
	sprintf(strToReturn, "0x%02X ", byteToConvert);
}

// Writes a String representing a hex of the given byte[] testByteArray 
/*Example usage:	
	uint8_t testByteArray[6] = {0, 1, 10, 11, 233, 73};
	char str_to_print_byteArray[50];
	byte_array_to_hexString(str_to_print_byteArray, testByteArray, 6);
	println(huart2, str_to_print_byteArray);*/
void byte_array_to_hexString(char* strToReturn, uint8_t* byteToConvert,  uint32_t len){
    unsigned char * pin = byteToConvert;
    const char * hex = "0123456789ABCDEF";
    char * pout = strToReturn;
    int i = 0;
    for(; i < len-1; ++i){
        *pout++ = hex[(*pin>>4)&0xF];
        *pout++ = hex[(*pin++)&0xF];
        *pout++ = ':';
    }
    *pout++ = hex[(*pin>>4)&0xF];
    *pout++ = hex[(*pin)&0xF];
    *pout = 0;
}

void floatToByteArray(uint8_t* byte_array, int start_pos, float float_to_convert){
  int length = sizeof(float);
  for(int i = 0; i < length; i++){
    byte_array[i+start_pos] = ((uint8_t*)&float_to_convert)[i];
  }
}

// Prints the RCT Time over UART2
void print_RTC_time(RTC_TimeTypeDef sTime){
	char text_to_print[25] = {0};
	sprintf(text_to_print,"RTC-Time: %u:%.2u:%.2u UTC",bcd_to_decimal(sTime.Hours), bcd_to_decimal(sTime.Minutes), bcd_to_decimal(sTime.Seconds));
	println(huart2, text_to_print);
}

// Prints the RCT Date over UART2
void print_RTC_date(RTC_DateTypeDef sDate){
	char text_to_print2[25] = {0};
	sprintf(text_to_print2,"RTC-Date: %.2u.%.2u.20%.2u UTC",bcd_to_decimal(sDate.Date), bcd_to_decimal(sDate.Month), bcd_to_decimal(sDate.Year));
	println(huart2, text_to_print2);
}

// Puts the RTC Date and Time in the given char*
void get_Rtc_date_time(RTC_HandleTypeDef* hrtc, RTC_TimeTypeDef sTime, RTC_DateTypeDef sDate, char* rtc_date_time){
		HAL_RTC_GetTime(hrtc, &sTime, RTC_FORMAT_BCD);
		HAL_RTC_GetDate(hrtc, &sDate, RTC_FORMAT_BCD);
	  sprintf(rtc_date_time,"%.2u:%.2u:%.2u,%.2u.%.2u.20%.2u",bcd_to_decimal(sTime.Hours), bcd_to_decimal(sTime.Minutes), bcd_to_decimal(sTime.Seconds),bcd_to_decimal(sDate.Date), bcd_to_decimal(sDate.Month), bcd_to_decimal(sDate.Year));
}

// Puts the RTC Date and Time in the given char*
void get_Rtc_date_time_for_raspi_sync(RTC_HandleTypeDef* hrtc, RTC_TimeTypeDef sTime, RTC_DateTypeDef sDate, char* rtc_date_time){
		HAL_RTC_GetTime(hrtc, &sTime, RTC_FORMAT_BCD);
		HAL_RTC_GetDate(hrtc, &sDate, RTC_FORMAT_BCD);

		float subsecs;
		subsecs = 1.0 - (1.0 / (bcd_to_decimal(sTime.SecondFraction)+1.0) )* bcd_to_decimal(sTime.SubSeconds);
		int subsecs_int = (int)(subsecs * 1000000);
		//bcd_to_decimal(sTime.SubSeconds), bcd_to_decimal(sTime.SecondFraction)
	
		sprintf(rtc_date_time,"20%.2u:%.2u:%.2u:%.2u:%.2u:%.2u:%d", bcd_to_decimal(sDate.Year), bcd_to_decimal(sDate.Month), bcd_to_decimal(sDate.Date), bcd_to_decimal(sTime.Hours), bcd_to_decimal(sTime.Minutes), bcd_to_decimal(sTime.Seconds), subsecs_int);
}

/*
  uint32_t SubSeconds;     !< Specifies the RTC_SSR RTC Sub Second register content.
                                 This parameter corresponds to a time unit range between [0-1] Second
                                 with [1 Sec / SecondFraction +1] granularity 

  uint32_t SecondFraction; 
*/

// Puts the RTC Date in the given char*
void get_Rtc_date(RTC_HandleTypeDef* hrtc, RTC_DateTypeDef sDate, char* rtc_date){
		HAL_RTC_GetDate(hrtc, &sDate, RTC_FORMAT_BCD);
	  sprintf(rtc_date,"%.2u%.2u%.2u",bcd_to_decimal(sDate.Date), bcd_to_decimal(sDate.Month), bcd_to_decimal(sDate.Year));
}

uint8_t bcd_to_decimal(uint8_t x) {
    return x - 6 * (x >> 4);
}

uint8_t decToBcd(uint8_t val)
{
  return( (val/10*16) + (val%10) );
}

float get_internal_temp_from_internal_adc(ADC_HandleTypeDef hadc) {
	//println(huart2, "READOUT ADC");
	HAL_ADC_Start(&hadc);
	//DWT_Delay_us(1000000);
	float TemperatureValue = 0.1;
	if (HAL_ADC_PollForConversion(&hadc, 1000) == HAL_OK) // Was: if (HAL_ADC_PollForConversion(&hadc, 1000000) == HAL_OK)
		{
			TemperatureValue = HAL_ADC_GetValue(&hadc);
			//println(huart2, "Wait for HAL_ADC_PollForConversion");
		}
	else{
		TemperatureValue = HAL_ADC_GetValue(&hadc);
		println(huart2, "Internal ADC Problem");
	}
	HAL_ADC_Stop(&hadc);
	
	
	/*float Vdd;
	Vdd = 3300 * (1.0)/ADC_raw; //VREFINT_CAL_ADDR
	return Vdd;*/
	  // ADC Conversion to read temperature sensor
  // Temperature (in °C) = ((Vsense – V25) / Avg_Slope) + 25
  // Vense = Voltage Reading From Temperature Sensor
  // V25 = Voltage at 25°C, for STM32F407 = 0.76V
  // Avg_Slope = 2.5mV/°C
  // This data can be found in the STM32F407VF Data Sheet

  TemperatureValue *= 3300.0f;
  TemperatureValue /= 0xfff; //Reading in mV
  TemperatureValue /= 1000.0f; //Reading in Volts
  TemperatureValue -= 0.760f; // Subtract the reference voltage at 25°C
  TemperatureValue /= .0025f; // Divide by slope 2.5mV
  TemperatureValue += 25.0f; // Add the 25°C
	return TemperatureValue;
}

