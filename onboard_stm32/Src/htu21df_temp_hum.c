// htu21df_temp_hum.c file
#include "htu21df_temp_hum.h"

//HAL_I2C_Master_Transmit(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout); 
//HAL_I2C_Master_Receive(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout); 

// USER VARIABLES
uint8_t i2c_buffer[5];  // I2C data buffer, only 3 needed
uint64_t rawT, rawH;    // raw values temperature, humidity
float Temperature;      // temperature value
float Humidity;         // humidity value

/**
  * @brief  Delay after soft-reset of HTU21DF via I2C interface. 
  *								Nominal value from datasheet: 15 ms.  
  */
void delay_softReset_HTU21DF(void){
	HAL_Delay(20);  // 15 ms
}

/**
  * @brief  Delay for temperature measurement from HTU21DF I2C interface. 
  *								Nominal max. value from datasheet, 14-bit: 50 ms.  
  */
void delay_meas_Temperature_HTU21DF(void){
	HAL_Delay(60);  // 50 ms
}

/**
  * @brief  Delay for humidity measurement from HTU21DF I2C interface. 
  *								Nominal max. value from datasheet, 12-bit: 16 ms.  
  */
void delay_meas_Humidity_HTU21DF(void){
	HAL_Delay(20);  // 20 ms
}

/**
  * @brief  Trigger temperature measurement from HTU21DF I2C interface. 
  * @param  i2c_handle Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  */
void trigger_meas_Temperature_HTU21DF(I2C_HandleTypeDef i2c_handle){
	//Trigger Temperature measurement 
	i2c_buffer[0]=HTU21DF_TRIGGER_NO_HOLD_TEMP_MEAS;  
	HAL_I2C_Master_Transmit(&i2c_handle,HTU21DF_DevAddress<<1,i2c_buffer,1,HTU21DF_I2C_TIMEOUT);  
}	

/**
  * @brief  Read temperature [C] from HTU21DF I2C interface. 
  * @param  i2c_handle Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval Temperature [C, float] - error value = 128.867.
  */
float read_meas_Temperature_HTU21DF(I2C_HandleTypeDef i2c_handle){
	/*
	Sensor disconnected: programm continues
	rawT = 0xFFFF
	Temperature = 128.867
	*/
	i2c_buffer[0]= 255; //MSB byte 
	i2c_buffer[1]= 255; //LSB byte  
	HAL_I2C_Master_Receive(&i2c_handle,HTU21DF_DevAddress<<1,i2c_buffer,2,HTU21DF_I2C_TIMEOUT);  
	//receive 2 bytes, store into i2c_buffer[0] and i2c_buffer[1]
	//i2c_buffer[0] : MSB data 
	//i2c_buffer[1] : LSB data 
	rawT = i2c_buffer[0]<<8 | i2c_buffer[1]; //combine 2 8-bit into 1 16bit  
	Temperature = (((float)rawT/65536.0f)*175.72f) - 46.85f; 
	return Temperature;
}	

/**
  * @brief  Trigger relative humidty measurement from HTU21DF I2C interface. 
  * @param  i2c_handle Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  */
void trigger_meas_Humidity_HTU21DF(I2C_HandleTypeDef i2c_handle){
	//Trigger Humidity measurement 
	i2c_buffer[0]= HTU21DF_TRIGGER_NO_HOLD_HUM_MEAS; 
	HAL_I2C_Master_Transmit(&i2c_handle,HTU21DF_DevAddress<<1,i2c_buffer,1,HTU21DF_I2C_TIMEOUT);  
}

/**
  * @brief  Read relative humidty [per cent] from HTU21DF I2C interface. 
  * @param  i2c_handle Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval Relative  humidity [per cent, float] - error value = 118.998.
  */
float read_meas_Humidity_HTU21DF(I2C_HandleTypeDef i2c_handle){
	/*
	Sensor disconnected: programm continues
	rawH = 0xFFFF
	Humidity = 118.998
	*/
	i2c_buffer[0]= 255; //MSB byte 
	i2c_buffer[1]= 255; //LSB byte  
	HAL_I2C_Master_Receive(&i2c_handle,HTU21DF_DevAddress<<1,i2c_buffer,2,HTU21DF_I2C_TIMEOUT);  
	//i2c_buffer[0] : MSB data 
	//i2c_buffer[1] : LSB data 
	rawH = i2c_buffer[0]<<8 | i2c_buffer[1]; //combine 2 8-bit into 1 16bit  
	Humidity = (((float)rawH/65536.0f)*125.0f) - 6.0f; // shift by bytes?
	//HAL_Delay(100);
	return Humidity;
}

/**
  * @brief  Soft-reset of HTU21DF via I2C interface. Recommended after start-up. 
  *                15 ms wait time before first measurement recommended.
  * @param  i2c_handle Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  */
void softReset_HTU21DF(I2C_HandleTypeDef i2c_handle){
	// Soft Reset HTU21DF: recommended once at start 
	i2c_buffer[0]=HTU21DF_SOFT_RESET;  
	HAL_I2C_Master_Transmit(&i2c_handle,HTU21DF_DevAddress<<1,i2c_buffer,1,HTU21DF_I2C_TIMEOUT);  
}

/**
  * @brief  Reads HTU21DF sensor user register via I2C interface. 
  * @param  i2c_handle Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval User register value (8-bit).
  */
uint8_t readUserReg_HTU21DF(I2C_HandleTypeDef i2c_handle){
	i2c_buffer[0]=0;  
	HAL_I2C_Master_Receive(&i2c_handle,HTU21DF_DevAddress<<1,i2c_buffer,1,HTU21DF_I2C_TIMEOUT);  
	return i2c_buffer[0];
}

/**
  * @brief  Initialise HTU21DF sensor via I2C interface. Recommended after start-up. 
  *                15 ms wait time before first measurement recommended.
  * @param  i2c_handle Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  */
void init_HTU21DF(I2C_HandleTypeDef i2c_handle){
	softReset_HTU21DF(i2c_handle);
	delay_softReset_HTU21DF(); // TO-DO - remove when other init process takes 20 ms!
	print_uart2("HTU21DF initialised.\r\n");
}

/**
  * @brief  Test of HTU21DF temperature and humidity measurement via I2C interface. 
  *                Prints to UART2.
  * @param  i2c_handle Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  */
void testPrint_HTU21DF(I2C_HandleTypeDef i2c_handle){
	// Temperature measurement
	print_uart2("HTU21DF: ");
	trigger_meas_Temperature_HTU21DF(i2c_handle);
	delay_meas_Temperature_HTU21DF();	
	float temp = read_meas_Temperature_HTU21DF(i2c_handle);
	char str_for_print[10];
	sprintf(str_for_print, "temp: %f", temp);
	print_uart2(str_for_print);
	print_uart2(", ");
	
	// Humidity measurement
	trigger_meas_Humidity_HTU21DF(i2c_handle);
	delay_meas_Humidity_HTU21DF();
	float hum = read_meas_Humidity_HTU21DF(i2c_handle);
	sprintf(str_for_print, "hum: %f", hum);
	print_uart2(str_for_print);
	print_uart2("\r\n");
}

void get_hum_temp_HTU21DF(I2C_HandleTypeDef i2c_handle, char* hum_temp_str){
	trigger_meas_Temperature_HTU21DF(i2c_handle);
	delay_meas_Temperature_HTU21DF();	
	float temp = read_meas_Temperature_HTU21DF(i2c_handle);
	trigger_meas_Humidity_HTU21DF(i2c_handle);
	delay_meas_Humidity_HTU21DF();
	float hum = read_meas_Humidity_HTU21DF(i2c_handle);
	sprintf(hum_temp_str, "%f,%f", temp, hum);	
}

/*
 * Usage in main.c
 * init_HTU21DF(hi2c1);  // check function-internal delay! 
 * while(1){
 *    testPrint_HTU21DF(hi2c1); // ideally split into meas. trigger, wait and receive
 *                                 for temperature and humidity.
 *                                 Opportunity to reduce conv. time, reduce bit res.
 * }
*/

/*
TO-DO list:
User register write function. 
NB: First read user register to capture reserved bits 3,4,5 and replay them correctly
in user reg. write.
Why use user register?
At lower resolution the conversion time drops significantly, while
the output data (conversion) remains the same.
*/
