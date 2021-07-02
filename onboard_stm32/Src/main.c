/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_lp_modes.h"
#include "fatfs.h"
#include "hx711_scale.h"
#include "utility_umweltspaeher.h"
#include "rtc_date_time.h"
#include "gps_fix.h"
#include "sd_io_logging.h"
#include "config.h"
#include "fdc1004_adc.h"
#include "htu21df_temp_hum.h"
#include "mcp3903_adc.h"
#include "display_i2c.h"
//#include "stm32f4xx_hal_adc.h"
//#include "stm32f4xx_hal_adc_ex.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

I2C_HandleTypeDef hi2c1; // For FDC and first temp and humiditysensor
I2C_HandleTypeDef hi2c3; // For second temp and humiditysensor

RTC_HandleTypeDef hrtc;

SD_HandleTypeDef hsd;

SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
UART_HandleTypeDef huart4;


TIM_HandleTypeDef htim2; // Timer for ADC (MCP3903) sampling

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
	
// FAT32 File System
FRESULT fr;
FATFS fs;
FIL fil;
UINT byteCount; // TODO Check if this is optional

// RTC Date and time
RTCDATETIME rtcDateTime;

// GPS Fix data
GPSFIX gpsFix;

// Config object
CONFIG config;

// Total loops that needs to be run
uint64_t totalLoops;

// Index for ADC Readouts
uint64_t i=0;

// ADC Data Buffer for writing to SD CARD

char adc_buffer_0[18432]; // 18*1024
//char adc_buffer_1[18432]; // For later use
uint32_t adc_buffer_0_index = 0;
uint32_t adc_buffer_1_index = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SDIO_SD_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2C3_Init(void);
static void MX_RTC_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART1_UART_Init_and_configure_gps(void);
static void MX_ADC1_Init(void);
static void MX_TIM2_Init(void);
void readOut_and_save_adc_data(FIL fil);
void standbyRTCMode(uint32_t seconds);
char* sample_telemetry(FIL fil, uint8_t* adc_values, int umweltspaeher_id);
uint32_t readAdcLTC1864(SPI_HandleTypeDef hspi_adc);
void initSpi(SPI_HandleTypeDef hspi_adc);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

// lifecycle state prototypes
funcptr 
	initialise(void), // Mount SD card and Initialise all sensor devices
	configure(void),	// Create config object with defaults and read config file if it exists
	test(void), 			// run all sensor tests OR
	execute(void), 		// execute the delay loop
	finalise(void), 	// perform cleanup and shutdown activities
	error(void);			// state for irrecoverable errors
	
void standbyRTCMode(uint32_t);
	
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* Lifecycle States ----------------------------------------------------------*/

funcptr initialise() {
	println(huart2, "-- Enter START State -----");
	
	// Initialise the Liquid Crystal Display
	LCM1602_Display_Init();
	printLCDStatic("Finding GPS signal", 0);
	
	// TODO IF WE HAVE A TIME DO NOT WAIT HERE
	println(huart2, "WAIT 300s FOR GPS");
	for (int gps_wait=0; gps_wait<1; gps_wait++){ // 300 is hardcoded. TAKE FROM CONFIG OBJECT TODO MORGAN
		HAL_Delay(1000);
		print(huart2, ".");
	}	
	
	// Print UUID for specific STM32
	char str[50];
	sprintf(str, "STM32 UUID: %d%d%d", STM32_UUID[0], STM32_UUID[1], STM32_UUID[2]);
	println(huart2, str);

	
  // Mount SD CARD
	if(f_mount(&fs, SD_Path, 1) == FR_OK){
		println(huart2, "Mounting File System was successful");
	}
	
	// set the date and time for RTC from GPS
	println(huart2, "BEGIN -  SET Date and Time");
	rtcDateTime = RTCDATETIME_Const(0);
	setRtcDateTimeFromGps(&rtcDateTime);
	rtcDateTime = RTCDATETIME_Const(0);
	setRtcDateTimeFromGps(&rtcDateTime);
	println(huart2, "END -  SET Date and Time");
	if (rtcDateTime.isSet) {
		println(huart2, "Date and Time has been set");
		char rtc_date_str[10]={0};
		get_Rtc_date(&hrtc, rtcDateTime.sDate, rtc_date_str);
		println(huart2, rtc_date_str);
		HAL_RTC_GetDate(&hrtc, &rtcDateTime.sDate, RTC_FORMAT_BCD);
	  print_RTC_date(rtcDateTime.sDate);
	}
	
	// set the GPS fix position from the GPS
	gpsFix = GPSFIX_Const(-1, "", "", 0, 0, "");
	setGpsFixFromGps(&gpsFix);
	if (gpsFix.fixTime != -1) {
		println(huart2, "GPS fix has been found and set");
		printLCDStatic("GPS Signal OK!", 0);
	}
	else{
		println(huart2, "NO GPS SIGNAL!");
		printLCDStatic("No GPS Signal!", 0);
	}

	println(huart2, "-- Exit START State -----");
	return (funcptr) configure;
}

funcptr configure() {
	println(huart2, "-- Enter CONFIGURE State -----");
	
	// TODO: Read config file
	// create config object	
	// Standardconfig: CONFIG_Const(6, 0, 23, 00, 10, -1, 10);
	#define STAR_THOURS_ 06 // Start Time hours in UTC
	#define START_MINUTES_ 00
	#define END_HOURS_ 22 // END Time hours in UTC
	#define END_MINUTES_ 00
	#define STBY_LOG_INTERVAL_ 20 // TODO MORGAN SOMETHING DOES NOT WORK WITH BIG VALUES like 300
	#define UMWELTSPAEHER_ID_ 102
	#define ADC_MCP3903_CONFIG_DELAY_ 10
	//#define LOOPS_FOR_TELEMETRY_SAMPLE_ = 40000 // After so many adc readouts there is a telemetry readout
	//#define LOOPS_FOR_NEW_FILE = 1000000


	config = CONFIG_Const(
		STAR_THOURS_,
		START_MINUTES_,
		END_HOURS_,
  	END_MINUTES_,
		STBY_LOG_INTERVAL_,
		UMWELTSPAEHER_ID_,
		ADC_MCP3903_CONFIG_DELAY_,
		360000, // LOOPS_FOR_TELEMETRY_SAMPLE_ 180000~5.5s // 360000 ~ 11s // 720000 ~ 22s
		3000000, // LOOPS_FOR_NEW_FILE  // @180000Hz: 1000000~55,55555555555556s // @360000Hz: 3000000~85s
		300); // WAITING TIME ONE STARt FOR GPS // TODO MORGEL // Put config object in initialise state
		
	if (rtcDateTime.isSet != 1) {	
		println(huart2, "RTC was not synchronised with GPS-time.");
		//return (funcptr) error
	}

		
	// Init timer (TIM2) for the ADC sampling and configure NIVC for that
	MX_TIM2_Init();
	HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0); // TODO check if needed
	HAL_NVIC_EnableIRQ(TIM2_IRQn); // TODO check if needed	
	println(huart2, "-- Exit CONFIGURE state -----");
	return (funcptr) execute;  // test execute // CONFIGURE TEST HERE

	
}

funcptr test() {
	println(huart2, "-- Enter TEST State -----");
	
	// string for testing
	char str[200];
	
	// Test for TImer2
	/*
	MX_TIM2_Init();
	
	HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0); // TODO check if needed
	HAL_NVIC_EnableIRQ(TIM2_IRQn); // TODO check if needed
	
	//Start TIM2
	println(huart2, "HAL_TIM_Base_Start_IT");
	if(HAL_TIM_Base_Start_IT(&htim2) != HAL_OK)
  {
		println(huart2, "ERROR - HAL_TIM_Base_Start_IT");
    Error_Handler();
  }
	println(huart2, "DONE - HAL_TIM_Base_Start_IT");
	

	
	// Stop TIM2
	println(huart2, "HAL_TIM_Base_Stop_IT");
	if (HAL_TIM_Base_Stop_IT(&htim2) != HAL_OK)
  {
		println(huart2, "ERROR - HAL_TIM_Base_Stop_IT");
    Error_Handler();
  }
	*/
	

	// Internal Temp-Sensor Test
	/*while(1){
		float temperature_internal = get_internal_temp_from_internal_adc(hadc1);
		sprintf(str, "Internal Temperature: %f", temperature_internal);
		println(huart2, str);
	}*/
	
	// CPU Speed
	//sprintf(str, "CPU Speed: %u", HAL_RCC_GetHCLKFreq());
	//println(huart2, str);
	
	// ADC test
	//testADC(hspi1);
	/*println(huart2, "ADC BENCHMARK - BEGIN");
	initSpi(hspi1);
	for(int i=0; i<1000000; i++){
		//readAdcLTC1864(hspi1);
		uint32_t adcVal = readAdcLTC1864(hspi1);
		sprintf(str, "%u:  ADC-LTC1864-0: %u, ADC-LTC1864-1: %u", adcVal, adcVal & ((1 << 16)-1), (adcVal >> 16));
		println(huart2, str);
	}
	println(huart2, "ADC BENCHMARK - DONE");*/
	
	// Test FDC1004
	// SETUP - FDC1004 REPEAT
	print_uart2("FDC1004 - REPEAT MODE\r\n");
	uint8_t FDC_1004_in_CAPDAC_val_ch1 = 0U;    
	uint8_t FDC_1004_in_CAPDAC_val_ch2 = 4U;
	float FDC_1004_physCapVal_ch1;
	float FDC_1004_physCapVal_ch2;
	fdc1004_init(hi2c1);
	// Set up Channel 1
	fdc1004_setup_SE_Meas_Ch1(hi2c1, FDC_1004_in_CAPDAC_val_ch1, FDC1004_CAPDAC_CONNECTED);
	// Set up Channel 2
	fdc1004_setup_SE_Meas_Ch2(hi2c1, FDC_1004_in_CAPDAC_val_ch2, FDC1004_CAPDAC_CONNECTED);
	// Trigger measurements - REPEAT MODE - both channels in one operation triggered!
	fdc1004_trigger_Meas(hi2c1, FDC1004_RATE_400HZ, (FDC1004_TRIGGER_CHANNELID_CH1 + FDC1004_TRIGGER_CHANNELID_CH2), FDC1004_REPEAT);
	while(1){
		//FDC_1004_in_CAPDAC_val_ch1++;
		//fdc1004_setup_SE_Meas_Ch1(hi2c1, FDC_1004_in_CAPDAC_val_ch1, FDC1004_CAPDAC_CONNECTED);
		//FDC_1004_physCapVal_ch1 = fdc1004_readMeasVal_Ch1(hi2c1, FDC_1004_in_CAPDAC_val_ch1);
		HAL_Delay(2);
		FDC_1004_physCapVal_ch1 = fdc1004_readMeasVal_channel_autoCapdac(hi2c1, 1);
		// Channel 2
		// Delay for new sample new read - 400 S/s - must be set properly to catch at correct rate
		//HAL_Delay(4);
		// read ch. 2 value
		FDC_1004_physCapVal_ch2 = fdc1004_readMeasVal_channel_autoCapdac(hi2c1, 2);
		char data_to_print[20];
		sprintf(data_to_print,  "%f   %f ", FDC_1004_physCapVal_ch1, FDC_1004_physCapVal_ch2);
		println(huart2, data_to_print);
	}
		
	// I2C Temperature and Humidity Sensor (HTU21DF) Test
/*
	while(1){
	  init_HTU21DF(hi2c1);
	  init_HTU21DF(hi2c3);
		println(huart2, "Sensor 1:");
		testPrint_HTU21DF(hi2c1);
		println(huart2, "Sensor 2:");
		testPrint_HTU21DF(hi2c3);
		println(huart2, "");
		println(huart2, "");
    HAL_Delay(500);
	}*/
		
	// SD card writer test
	//testSdFat();

	// RTC Date and Time Test
	sprintf(str, "rtcDateTime.isSet: %d, ", rtcDateTime.isSet);
	println(huart2, str);
	HAL_RTC_GetTime(&hrtc, &rtcDateTime.sTime, RTC_FORMAT_BCD);
	print_RTC_time(rtcDateTime.sTime);
	HAL_RTC_GetDate(&hrtc, &rtcDateTime.sDate, RTC_FORMAT_BCD);
	print_RTC_date(rtcDateTime.sDate);
	
	// GPS Fix Test
	sprintf(str, "gpsFix: %d, %s, %s, %d, %d, %s", 
		gpsFix.fixTime, 
		gpsFix.latitude, 
		gpsFix.longitude, 
		gpsFix.fixQuality, 
		gpsFix.numSatellites, 
		gpsFix.altitude);
	println(huart2, str);

	println(huart2, "-- Exit TEST State -----");
	return (funcptr) finalise;
}

funcptr execute() {
	println(huart2, "-- Enter EXECUTE State -----");
	
	uint64_t readouts_per_file = config.loops_for_new_file; // After this number of ADC-readouts a new file is created // Was for test on 23.02.2018 10000000
	uint64_t number_of_total_adc_readouts = totalLoops; //10000000000; // Total number of ADC-readouts (can be calculated from the time it should run (seconds * 5000)) // was for test on 23.02.2018 1000000000000;
	int umweltspaeher_id = config.umweltspaeher_id; // -1 for TESTVERSION
	
	int file_number = 0; // TODO detect file son SD card and increment this number until there is no other file with the same name
	// Main loop for umweltspaeher
	init_adc_continuousReading_config(hspi1);
	uint8_t adc_values[18]; // 6 x 24 bit values from ADC
	
	// To create a new file
	uint64_t index_at_last_readouts_per_file = readouts_per_file; // SO in first round a new file will be created
	uint64_t index_at_last_saving_telemetry = 0; // For saving telemetry data
	uint64_t index_at_last_saving_fdc = 0; // For saving fdc data
	
	// Start Timer for ADC sampling (TIM2)
	__disable_irq(); // Disable IRQ because there is no textfile to save the adc data
	println(huart2, "HAL_TIM_Base_Start_IT");
	if(HAL_TIM_Base_Start_IT(&htim2) != HAL_OK)
  {
		println(huart2, "ERROR - HAL_TIM_Base_Start_IT");
    // TODO Go to error state //Error_Handler();
  }
	while (1){
		if (i > 36000*60*20){ //(i == 36000*60*20){ // TODO TEST // After 20 minutes reset
			__disable_irq();
			f_close(&fil);
			__enable_irq();
			break;
		}
		/*char str_to_printt[50];
		sprintf(str_to_printt, " i=%u", i);
		print(huart2, str_to_printt);*/
		
		__disable_irq();
		if((i-index_at_last_readouts_per_file) >= readouts_per_file){
			println(huart2, "Need to create new file.");
			index_at_last_readouts_per_file = i;
			if (i>0){
				f_close(&fil);
			}
			println(huart2, "Closed File.");
			char filename[50];
			char rtc_date_str[10]={0};
			get_Rtc_date(&hrtc, rtcDateTime.sDate, rtc_date_str);
			//println(huart2, rtc_date_str);
			sprintf(filename,  "%s0%.1d.%.3d", rtc_date_str, file_number/1000, file_number%1000);
			
			//file_number += 1;
			// Increment file_number as long as antother file with the same name exist
			while (does_file_exist(&fil, filename)){
				file_number += 1;
				sprintf(filename,  "%s0%.1d.%.3d", rtc_date_str, file_number/1000, file_number%1000);
			}
			println(huart2, filename);
			fr = open_append(&fil, filename);
			if (fr != FR_OK) {
				println(huart2, "open_append fail");
			  char str_to_printt[50];
				sprintf(str_to_printt, "filename:%s     error: fr=%u\n", filename, fr);
				print(huart2, str_to_printt);
				//return 1;
				return (funcptr) error;
			}
		}
		__enable_irq();
		
		if (adc_buffer_0_index >= 18*1024){
			//println(huart2, "BEGIN WRITE TO SD");	
			// Write data to SD
			__disable_irq();
			fr = f_write(&fil, adc_buffer_0, 18*1024, &byteCount);
			if (fr != FR_OK) {
				char error_str[50];
				sprintf(error_str, "f_write - ERROR: %d" , fr);
				println(huart2, error_str);	
			}
			adc_buffer_0_index = 0;
			__enable_irq();
			//println(huart2, "END WRITE TO SD");	
		}
		
		
	 

		/*if (i % 1 == 0) {
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET); // CS for SPI1 GPIO ON 1
			init_adc_continuousReading_config(hspi1);
		}*/
		// Get ADC values // Now done with a timer
		/*DWT_Delay_us(adc_mcp3903_read_delay);
		readoutADC_continuously(hspi1, adc_values);
		// Write data to SD
		__disable_irq();
		f_write(&fil, adc_values, 18, &byteCount);
		__enable_irq(); */
		
		// ADC Testprint for ben BEGIN TODO REMOVE 
		/*if(i%1000==0){
			for (int k=0; k<18; k+=3){
				char str_for_print[20];
				sprintf(str_for_print, " ; %u ", 256*256*adc_values[k]+256*adc_values[k+1]+adc_values[k+2]);
				print_uart2(str_for_print);
			}
		print_uart2("\r\n");
		}*/
		// ADC Testprint END
		
				// Get and Save telemetry data
		if((i-index_at_last_saving_telemetry) > config.loops_for_telemetry){ //40000
				print_free_memory();
				uint8_t adc_values[18];
				for (int x = 0; x <18; x++){
					adc_values[x] = x+65;
				}
				// Stop TIM2 (ADC sampling) to sample telemetry configure ADC again
				if (HAL_TIM_Base_Stop_IT(&htim2) != HAL_OK)
				{
					println(huart2, "ERROR - HAL_TIM_Base_Stop_IT");
					Error_Handler();
				}
				println(huart2, "TELEMETRY BEGIN: HAL_TIM_Base_Stop_IT");
				// Set the date and time for RTC from GPS
				println(huart2, "BEGIN -  SET Date and Time");
				//rtcDateTime = RTCDATETIME_Const(0);
				//for (int index=0; index<100; index++){  // DEBUGGING REMOVE
				setRtcDateTimeFromGps(&rtcDateTime);
				//}
				println(huart2, "END -  SET Date and Time");
				if (rtcDateTime.isSet) {
					println(huart2, "Date and Time has been set");
				}
				
				// Show date and time on display
				char time_string[50] ={0};
			  get_Rtc_date_time(&hrtc, rtcDateTime.sTime, rtcDateTime.sDate, time_string);
				printLCDStatic(time_string, 1);

				char* telemetry_string_ptr = sample_telemetry(fil, adc_values, umweltspaeher_id);
				// TODO find out why writing to sd card does not work in this function when we call it from here (in main loop it works)
				__disable_irq();
				f_write(&fil, adc_values, 18, &byteCount);
				f_write(&fil, telemetry_string_ptr, 352, &byteCount); // TODO: USE sth likesizeof(telemetry_string) 352 ost hardcoded!
				f_write(&fil, adc_values, 18, &byteCount);
				__enable_irq();
			
			  index_at_last_saving_telemetry = i;
			
				// Reconfigure ADC mcp3903 // Could have caused problems on Neurocopter
				//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET); // CS for SPI1 GPIO ON 1
				//init_adc_continuousReading_config(hspi1);
				DWT_Delay_us(config.adc_mcp3903_config_delay);
				println(huart2, "TELEMETRY END: HAL_TIM_Base_Start_IT");
				// Start TIM2
				DWT_Delay_us(10000);
				if(HAL_TIM_Base_Start_IT(&htim2) != HAL_OK)
				{
					println(huart2, "ERROR - HAL_TIM_Base_Start_IT");
					Error_Handler();
				}
		}
		
		//f_printf(&fil, "\r\n"); // Takes to much time
			/*for (int k=0; k<18; k++){
				char str_for_print[20];
				sprintf(str_for_print, " ; %u ", adc_values[k]);
				print_uart2(str_for_print);
			}
		print_uart2("\r\n");*/
	}
	//println(huart2, "end adc readout");
	
	// Close File
	println(huart2, "Close File");
	__disable_irq();
  f_close(&fil); // TODO: Test if that causes a problem if the file is already closed
	__enable_irq();
	
	println(huart2, "-- Exit EXECUTE State -----");
	return (funcptr) finalise;
}


funcptr finalise() {
	println(huart2, "-- Enter FINALISE State -----");
	
	// free memory allocated on the heap for scale structures
	//HX711_Dest(scale1);
	//HX711_Dest(scale2);
	//HX711_Dest(scale3);
	
	// free memory allocated on the heap for the config structure
	//CONFIG_Dest(config);
	
	// free allocated memory on the heap for the gps fix structure
	//GPSFIX_Dest(gpsFix);
	
	// free memory allocated on the heap for the rtc date time structure
	//RTCDATETIME_Dest(rtcDateTime);
	
	println(huart2, "Entering standby mode");
  char strToPr[50];
	sprintf(strToPr, "config.stby_log_interval: %u", config.stby_log_interval);
	println(huart2, strToPr);
	NVIC_SystemReset(); // Reset the MCU
	//standbyRTCMode(config.stby_log_interval);
	
	__enable_irq(); // TODO test if it helped
	println(huart2, "-- Exit FINALISE State -----");
	return (funcptr) initialise;
}

funcptr error() {
	// TODO: give meaningful error message and find some way to shutdown the board
  println(huart2, "An error has occured");
	println(huart2, "Reset the MCU");
	NVIC_SystemReset(); // Reset the MCU
	return (funcptr) initialise;
}

/* USER CODE END 0 */

/**
* @brief This function handles TIM2 global interrupt. It is used for the MCP3903 ADC.
*/
void TIM2_IRQHandler(void)
{
	HAL_NVIC_ClearPendingIRQ(TIM2_IRQn);
	HAL_TIM_IRQHandler(&htim2);
	// Do ADC Measurement and Save it to SD CARD
	readOut_and_save_adc_data(fil);
}

// for UART 1 receiving
/*void USART1_IRQHandler(void)
{
	println(huart2, "Received via uart1");	
}*/

// Readouts ADC and writes the data to the buffer
// Called in TIM2-interrupt handler
void readOut_and_save_adc_data(FIL fil){
	//print(huart2, ".");
	uint32_t adcValues = readAdcLTC1864(hspi1);
	uint8_t adc_values[4]; // 2 x 16 bit values from ADC
	adc_values[0] = (adcValues >> 24) & 0xFF;
	adc_values[1] = (adcValues >> 16) & 0xFF;
	adc_values[2] = (adcValues >> 8) & 0xFF;
	adc_values[3] = adcValues & 0xFF;
	//readoutADC_continuously(hspi1, adc_values);
	// Write data to buffer
	if(adc_buffer_0_index <= (18432-4)){
		for (int j=0; j<4; j++){
			adc_buffer_0[adc_buffer_0_index+j] = adc_values[j];
		}
		//adc_buffer_0; // Buffer Size = 18432
		adc_buffer_0_index += 4;
	}
	i++;
}

// This is not nice...
#define SPI_I2S_FLAG_RXNE               ((uint16_t)0x0001)
#define SPI_I2S_FLAG_TXE                ((uint16_t)0x0002)
#define SPI_I2S_FLAG_BSY                ((uint16_t)0x0080)

uint16_t SPI1_send(uint16_t data){
	SPI1->DR = data; // write data to be transmitted to the SPI data register
	while( !(SPI1->SR & SPI_FLAG_TXE) ); // wait until transmit complete
	while( !(SPI1->SR & SPI_FLAG_RXNE) ); // wait until receive complete
	while( SPI1->SR & SPI_FLAG_BSY ); // wait until SPI is not busy anymore
	return SPI1->DR; // return received data from SPI data register
}

// Inits the SPI
void initSpi(SPI_HandleTypeDef hspi_adc){
	uint8_t spiRxBuffTmp[2];
	uint8_t Byte_NastrTmp=0;
	HAL_SPI_TransmitReceive(&hspi_adc, &Byte_NastrTmp, spiRxBuffTmp, 2, 50); // Receive 3 bytes (24 Bit from  ADC register)
}

// Reads a 16 bit register of the ADC LTC1864
uint16_t adc0Value;
uint16_t adc1Value;
uint32_t adcValues;
uint32_t readAdcLTC1864(SPI_HandleTypeDef hspi_adc){ // uint8_t reg
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); // CS0 for SPI1 GPIO ON 0
	//println(huart2, ".");
	//GPIOA->BSRR |= GPIO_PIN_4;
	//HAL_SPI_TransmitReceive(&hspi_adc, &Byte_Nastr, spiRxBuff, 2, 50); // Receive 3 bytes (24 Bit from  ADC register)
	//HAL_SPI_Receive(&hspi_adc, spiRxBuff, 2, 1);
	adcValues = SPI1_send(0x00) << 16;
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET); // CS0 for SPI1 GPIO ON 1
	//println(huart2, ".");
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET); // CS1 for SPI1 GPIO ON 0
	//println(huart2, ".");
	adc1Value = SPI1_send(0x00);
	//println(huart2, ".");
	adcValues = adcValues + adc1Value; //  << 16;
	//char str[200];
	//sprintf(str, "ADC-LTC1864-1: %u", adc1Value);
	//println(huart2, str);
	
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); // CS0 for SPI1 GPIO ON 0
	//println(huart2, ".");
	SPI1_send(0x00); // Fake readout to reset adc0
	//println(huart2, ".");
	
	// Start measurement simultaneously
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0|GPIO_PIN_4, GPIO_PIN_SET); // CS0 for SPI1 GPIO ON 1
	//println(huart2, ".");
	//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET); // CS1 for SPI1 GPIO ON 0
	//GPIOA->BSRR |= GPIO_PIN_4;
	
	//uint32_t registerValue = spiRxBuff[0] << 8;
	//registerValue += spiRxBuff[1];
	return adcValues;
}

// Readout a single channel of the adc
/*uint32_t readAdcLTC1864(SPI_HandleTypeDef hspi_adc){ // uint8_t reg
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET); // CS0 for SPI1 GPIO ON 0
	adcValues = SPI1_send(0x00) << 16;
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET); // CS0 for SPI1 GPIO ON 1
	return adcValues;
}*/

char* sample_telemetry(FIL fil_telemetry, uint8_t* adc_values, int umweltspaeher_id){
			print_uart2("Telemetry BEGIN\r\n");

	// Mark first 18 bytes which will be written to the file as an Identifier (ABCDEFGH..) at the BEGINING and END of the String
			for (int x = 0; x <18; x++){
				adc_values[x] = x+65;
			}
			
			// Print timestring for raspberry pi camera synchronisation on usart3
			char time_string[50] ={0};
			get_Rtc_date_time_for_raspi_sync(&hrtc, rtcDateTime.sTime, rtcDateTime.sDate, time_string);
			println(huart3, time_string);
			//print_uart2("END RTC\r\n");
			//print_uart2("BEGIN get_hum_temp_HTU21DF\r\n");
			char temp_hum_string[50] ={0};
			// Init hum and temps sensors in every loop (if there is a hardware error it can be fixed threw this)
			init_HTU21DF(hi2c1);
	    init_HTU21DF(hi2c3);
			get_hum_temp_HTU21DF(hi2c1, temp_hum_string);
			//print_uart2("END get_hum_temp_HTU21DF\r\n");
			//print_uart2("BEGIN scale HX711\r\n");
			//print_uart2("END scale HX711\r\n");
			char gps_pos_data_string[100] ={0};
			sprintf(gps_pos_data_string, "%d, %s, %s, %d, %d, %s", 
			gpsFix.fixTime, gpsFix.latitude, gpsFix.longitude, gpsFix.fixQuality, gpsFix.numSatellites, gpsFix.altitude);
			//println(huart2, gps_pos_data_string);
			
			char internal_temp_data_string[20] ={0};
			float temperature_internal = get_internal_temp_from_internal_adc(hadc1);
			sprintf(internal_temp_data_string, "%f", temperature_internal);
		
			
		 char uuid_string[50];
	   sprintf(uuid_string, "%d%d%d", STM32_UUID[0], STM32_UUID[1], STM32_UUID[2]);
			
			char telemetry_string[352] ={0};
			sprintf(telemetry_string,"%d,%s,%s,%s,%s,%s", umweltspaeher_id, time_string, temp_hum_string, gps_pos_data_string, internal_temp_data_string, uuid_string);
			print_uart2(telemetry_string);
			__disable_irq();
			int write_result_0 = f_write(&fil_telemetry, adc_values, 18, &byteCount);
			int write_result_1 = f_write(&fil_telemetry, telemetry_string, sizeof(telemetry_string), &byteCount);
			int write_result_2 = f_write(&fil_telemetry, adc_values, 18, &byteCount);
			char strToPrintWrite[50];
			sprintf(strToPrintWrite, "\r\n write_results: %d %d %d size: %d\r\n", write_result_0, write_result_1, write_result_2, sizeof(telemetry_string));
			print_uart2(strToPrintWrite);
			__enable_irq();
			//print_uart2("Telemetry END\r\n");
			return telemetry_string;
}

int main(void) {
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
	DWT_Delay_Init();
	
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
	/* Enable Power Clock */
  __PWR_CLK_ENABLE();

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SDIO_SD_Init();
  MX_USART2_UART_Init();
	MX_USART3_UART_Init();
  MX_FATFS_Init();
  MX_I2C1_Init();
	MX_I2C3_Init();
  MX_RTC_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init_and_configure_gps();
	MX_ADC1_Init();

  /* USER CODE BEGIN 2 */
	/* Check and handle if the system was resumed from StandBy mode */
  if (__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET) {
		println(huart2, "Starting system after Reset");
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
  }
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	
	// set the initial state
	ptrfuncptr state = initialise;
	
	while (1) {
  /* USER CODE END WHILE */		
  
	/* USER CODE BEGIN 3 */
		
		state = (ptrfuncptr)(*state)();
  }
  /* USER CODE END 3 */
}

/** System Clock Configuration */
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;
	
	/* Enable Power Control clock */
  __PWR_CLK_ENABLE();

	/**Configure the main internal regulator output voltage 
	*/
	__HAL_RCC_PWR_CLK_ENABLE();

	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                               |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
    _Error_Handler(__FILE__, __LINE__);
  }

  /**Configure the Systick interrupt time */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  /**Configure the Systick */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* I2C1 init function */
static void MX_I2C1_Init(void) {
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK) {
    _Error_Handler(__FILE__, __LINE__);
  }
}


/* I2C1 init function */
static void MX_I2C3_Init(void)
{

  hi2c3.Instance = I2C3;
  hi2c3.Init.ClockSpeed = 100000;
  hi2c3.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c3.Init.OwnAddress1 = 0;
  hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c3.Init.OwnAddress2 = 0;
  hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* RTC init function */
static void MX_RTC_Init(void) {
  /**Initialize RTC Only */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK) {
    _Error_Handler(__FILE__, __LINE__);
  }

  /**Initialize RTC and set the Time and Date */
  if (HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR0) != 0x32F2) {
		rtcDateTime.sTime.Hours = 0x0;
		rtcDateTime.sTime.Minutes = 0x0;
		rtcDateTime.sTime.Seconds = 0x0;
		rtcDateTime.sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
		rtcDateTime.sTime.StoreOperation = RTC_STOREOPERATION_RESET;
		if (HAL_RTC_SetTime(&hrtc, &rtcDateTime.sTime, RTC_FORMAT_BCD) != HAL_OK) {
			_Error_Handler(__FILE__, __LINE__);
		}

		rtcDateTime.sDate.WeekDay = RTC_WEEKDAY_MONDAY;
		rtcDateTime.sDate.Month = RTC_MONTH_JANUARY;
		rtcDateTime.sDate.Date = 0x1;
		rtcDateTime.sDate.Year = 0x0;
		if (HAL_RTC_SetDate(&hrtc, &rtcDateTime.sDate, RTC_FORMAT_BCD) != HAL_OK) {
			_Error_Handler(__FILE__, __LINE__);
		}
    HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR0,0x32F2);
  }
}

/* SDIO init function */
static void MX_SDIO_SD_Init(void) {
  hsd.Instance = SDIO;
  hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
  hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
  hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
  hsd.Init.BusWide = SDIO_BUS_WIDE_1B;
  hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd.Init.ClockDiv = 0;
}

/* ADC1 init function */
static void MX_ADC1_Init(void) {
  ADC_ChannelConfTypeDef sConfig;

  /**Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion) */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK) {
    _Error_Handler(__FILE__, __LINE__);
  }

  /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. */
  sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
    _Error_Handler(__FILE__, __LINE__);
  }
}

/* SPI1 init function */
static void MX_SPI1_Init(void) {
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_16BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW; // SPI_POLARITY_HIGH in datasheet of adc1864
  hspi1.Init.CLKPhase = SPI_PHASE_2EDGE; // SPI_PHASE_2EDGE in datasheet of adc1864
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8; // USE: SPI_BAUDRATEPRESCALER_8 RESULTS in 10.5 MHz - 16.7 MHz is the limit of the LTC1864-ADC // WAS SPI_BAUDRATEPRESCALER_2 (=42.0 MHz);
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK) {
    _Error_Handler(__FILE__, __LINE__);
  }
}

/* USART1 init function */
static void MX_USART1_UART_Init_and_configure_gps(void) {	
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;  // For old GPS use: 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK) {
    _Error_Handler(__FILE__, __LINE__);
  }
	
	//println(huart2, "Set GPS baudrate to 115200");
	// Set GPS baudrate to 115200
	HAL_UART_Transmit(&huart1, (uint8_t *) "$PMTK251,115200*1F\r\n", 20, 50);
	
	//println(huart2, "Set UART1 baudrate to 115200");
	huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;  // For old GPS use: 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK) {
    _Error_Handler(__FILE__, __LINE__);
  }
	
	//println(huart2, "Set GPS refresh rate to 10Hz");
	// Set GPS refresh rate to 10Hz
	HAL_UART_Transmit(&huart1, (uint8_t *) "$PMTK300,100,0,0,0,0*2C\r\n", 25, 50);
	
}

/* USART2 init function */
static void MX_USART2_UART_Init(void) {
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK) {
    _Error_Handler(__FILE__, __LINE__);
  }
}

/* USART3 init function */
static void MX_USART3_UART_Init(void) {
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK) {
    _Error_Handler(__FILE__, __LINE__);
  }
}



/* TIM2 init function */
// TODO: Make frequency configurable.. it is now arround: 5.02khz
void MX_TIM2_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 2256; // 4506 for 18000Hz // Result with TIM_CLOCKDIVISION_DIV4 --> 75020 Hz //1128
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV4; // Was: TIM_CLOCKDIVISION_DIV1;
  HAL_TIM_Base_Init(&htim2);

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig);

}



/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : SPI1_CS0_Pin */
  GPIO_InitStruct.Pin = SPI1_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(SPI1_CS_GPIO_Port, &GPIO_InitStruct);

	// SPI1 CS1 Pin
  //Configure GPIO pin : PA0 
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PE3, PE5  */
  GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_5; // Had GPIO_PIN_10
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	 GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	 /* DEBUG // TODO MAKE NICE // Configure GPIO pins : */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_4|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	
		 /* DEBUG // TODO MAKE NICE // Configure GPIO pins : PE13
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct); */
	
	/*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3|GPIO_PIN_5, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
}

/* USER CODE BEGIN 4 */

/**
  * @brief  Wake Up Timer callback
  * @param  hrtc : hrtc handle
  * @retval None
  */
void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc) { 
  println(huart2, "HAL_RTCEx_WakeUpTimerEventCallback");
}

/**
  * @brief  This function configures the system to enter Standby mode with RTC 
  *         clocked by LSE or LSI for current consumption measurement purpose.
  *         STANDBY Mode with RTC clocked by LSE/LSI
  *         ========================================
  *           - RTC Clocked by LSE/LSI
  *           - IWDG OFF
  *           - Backup SRAM OFF
  *           - Automatic Wakeup using RTC clocked by LSE/LSI (after ~20s)
  * @param  Number of seconds to stay in standby mode
  * @retval None
  */
void standbyRTCMode(uint32_t seconds) {
  hrtc.Instance = RTC;  
  /* Configure RTC prescaler and RTC data registers as follow:
  - Hour Format = Format 24
  - Asynch Prediv = Value according to source clock
  - Synch Prediv = Value according to source clock
  - OutPut = Output Disable
  - OutPutPolarity = High Polarity
  - OutPutType = Open Drain */ 
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
  hrtc.Init.SynchPrediv = RTC_SYNCH_PREDIV;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  
  if(HAL_RTC_Init(&hrtc) != HAL_OK) {
    /* Initialization Error */
    Error_Handler(); 
  }
  
  /*## Configure the Wake up timer ###########################################*/
  /*  RTC Wakeup Interrupt Generation:
      Wakeup Time Base = (RTC_WAKEUPCLOCK_RTCCLK_DIV /(LSI))
      Wakeup Time = Wakeup Time Base * WakeUpCounter 
                  = (RTC_WAKEUPCLOCK_RTCCLK_DIV /(LSI)) * WakeUpCounter
      ==> WakeUpCounter = Wakeup Time / Wakeup Time Base

      To configure the wake up timer to 20s the WakeUpCounter is set to 0xA017:
        RTC_WAKEUPCLOCK_RTCCLK_DIV = RTCCLK_Div16 = 16 
        Wakeup Time Base = 16 /(~32.768KHz) = ~0,488 ms
        Wakeup Time = ~20s = 0,488ms  * WakeUpCounter
        ==> WakeUpCounter = ~20s/0,488ms = 40983 = 0xA017 */
	
	double wakeUpTimeBase = 16 / ((double) LSI_VALUE / 1000);
	uint32_t wakeUpCounter = (seconds * 1000) / wakeUpTimeBase;
	
  /* Disable Wake-up timer */
  if (HAL_RTCEx_DeactivateWakeUpTimer(&hrtc) != HAL_OK) {
    /* Initialization Error */
    Error_Handler(); 
  }
  
  /*#### Clear all related wakeup flags ######################################*/
  /* Clear PWR wake up Flag */
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
  
  /* Clear RTC Wake Up timer Flag */
  __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&hrtc, RTC_FLAG_WUTF);
  
  /*#### Setting the Wake up time ############################################*/
  HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, wakeUpCounter, RTC_WAKEUPCLOCK_RTCCLK_DIV16);
  
  /*#### Enter the Standby mode ##############################################*/
  /* Request to enter STANDBY mode  */
  HAL_PWR_EnterSTANDBYMode(); 
}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char * file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1) {
  }
  /* USER CODE END Error_Handler_Debug */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
