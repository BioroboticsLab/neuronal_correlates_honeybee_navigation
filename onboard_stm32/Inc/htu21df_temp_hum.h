// htu21df_temp_hum.h file
#ifndef htu21df_temp_hum_h
#define htu21df_temp_hum_h

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"
#include "utility_umweltspaeher.h"

static const uint8_t HTU21DF_DevAddress = (0x40);
static const uint8_t HTU21DF_TRIGGER_NO_HOLD_TEMP_MEAS = (0xF3);
static const uint8_t HTU21DF_TRIGGER_NO_HOLD_HUM_MEAS = (0xF5);
// Do not use - blocks I2C bus during HTU21DF measurement
static const uint8_t HTU21DF_TRIGGER_HOLD_TEMP_MEAS = (0xE3); 
// Do not use - blocks I2C bus during HTU21DF measurement
static const uint8_t HTU21DF_TRIGGER_HOLD_HUM_MEAS = (0xE5); 
static const uint8_t HTU21DF_SOFT_RESET = (0xFE);
static const uint8_t HTU21DF_WRITE_USER_REG = (0xE6);
static const uint8_t HTU21DF_READ_USER_REG = (0xE7);

static const uint8_t HTU21DF_I2C_TIMEOUT = 10; //ms

void delay_meas_Temperature_HTU21DF(void);
void delay_meas_Humidity_HTU21DF(void);
void delay_softReset_HTU21DF(void);
void softReset_HTU21DF(I2C_HandleTypeDef i2c_handle);
void init_HTU21DF(I2C_HandleTypeDef i2c_handle);
void trigger_meas_Temperature_HTU21DF(I2C_HandleTypeDef i2c_handle);
void trigger_meas_Humidity_HTU21DF(I2C_HandleTypeDef i2c_handle);
float read_meas_Temperature_HTU21DF(I2C_HandleTypeDef i2c_handle);
float read_meas_Humidity_HTU21DF(I2C_HandleTypeDef i2c_handle);
uint8_t readUserReg_HTU21DF(I2C_HandleTypeDef i2c_handle);
void testPrint_HTU21DF(I2C_HandleTypeDef i2c_handle);
void get_hum_temp_HTU21DF(I2C_HandleTypeDef i2c_handle, char* hum_temp_str);


#endif /* htu21df_temp_hum_h */ 
