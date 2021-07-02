// fdc1004_adc.h file
#ifndef fdc1004_adc_h
#define fdc1004_adc_h

#include <stdio.h> // TO-DO necessary?
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"
#include "utility_umweltspaeher.h"

// Dev. Address
static const uint8_t FDC1004_DevAddress = (0x50); // 0b1010000
static const uint8_t FDC1004_I2C_TIMEOUT = 1; //ms

//Read registers - ADDRESSES
static const uint8_t FDC1004_ADDR_MEAS1_MSB = (0x00);
static const uint8_t FDC1004_ADDR_MEAS1_LSB = (0x01);
static const uint8_t FDC1004_ADDR_MEAS2_MSB = (0x02);
static const uint8_t FDC1004_ADDR_MEAS2_LSB = (0x03);
static const uint8_t FDC1004_ADDR_MEAS3_MSB = (0x04);
static const uint8_t FDC1004_ADDR_MEAS3_LSB = (0x05);
static const uint8_t FDC1004_ADDR_MEAS4_MSB = (0x06);
static const uint8_t FDC1004_ADDR_MEAS4_LSB = (0x07);

//Read/Write Registers - ADDRESSES
static const uint8_t FDC1004_ADDR_CONF_MEAS1 = (0x08);
static const uint8_t FDC1004_ADDR_CONF_MEAS2 = (0x09);
static const uint8_t FDC1004_ADDR_CONF_MEAS3 = (0x0A);
static const uint8_t FDC1004_ADDR_CONF_MEAS4 = (0x0B);

static const uint8_t FDC1004_ADDR_FDC_CONF = (0x0C);

static const uint8_t FDC1004_ADDR_OFFSET_CAL_CIN1 = (0x0D);
static const uint8_t FDC1004_ADDR_OFFSET_CAL_CIN2 = (0x0E);
static const uint8_t FDC1004_ADDR_OFFSET_CAL_CIN3 = (0x0F);
static const uint8_t FDC1004_ADDR_OFFSET_CAL_CIN4 = (0x10);

static const uint8_t FDC1004_ADDR_GAIN_CAL_CIN1 = (0x11);
static const uint8_t FDC1004_ADDR_GAIN_CAL_CIN2 = (0x12);
static const uint8_t FDC1004_ADDR_GAIN_CAL_CIN3 = (0x13);
static const uint8_t FDC1004_ADDR_GAIN_CAL_CIN4 = (0x14);

// Read-only registers - ADDRESSES
static const uint8_t FDC1004_ADDR_MANUFACTURE_ID = (0xFE);
static const uint8_t FDC1004_ADDR_DEVICE_ID = (0xFF);

// Constants for configuration
static const uint8_t FDC1004_RATE_100HZ = (0x01);
static const uint8_t FDC1004_RATE_200HZ = (0x02);
static const uint8_t FDC1004_RATE_400HZ = (0x03);

static const uint8_t FDC1004_CHANNELID_CH1 = (0x00);
static const uint8_t FDC1004_CHANNELID_CH2 = (0x01);
static const uint8_t FDC1004_CHANNELID_CH3 = (0x02);
static const uint8_t FDC1004_CHANNELID_CH4 = (0x03);

static const uint8_t FDC1004_CHANNELID_TO_CAPDAC = (0x04);
static const uint8_t FDC1004_CHANNELID_DISABLED = (0x07);

static const uint8_t FDC1004_CAPDAC_MAX = (0x1F);
static const uint8_t FDC1004_CAPDAC_MIN = (0x00);

static const uint8_t FDC1004_RST = (0x01);
static const uint8_t FDC1004_REPEAT = (0x01);
static const uint8_t FDC1004_NO_REPEAT = (0x00);
static const uint8_t FDC1004_CAPDAC_CONNECTED = (0x01);
static const uint8_t FDC1004_CAPDAC_NOT_CONNECTED = (0x00);

static const uint8_t FDC1004_TRIGGER_CHANNELID_CH1 = (0x08);
static const uint8_t FDC1004_TRIGGER_CHANNELID_CH2 = (0x04);
static const uint8_t FDC1004_TRIGGER_CHANNELID_CH3 = (0x02);
static const uint8_t FDC1004_TRIGGER_CHANNELID_CH4 = (0x01);

// FUNCTIONS
HAL_StatusTypeDef fdc1004_write_register(I2C_HandleTypeDef i2c_handle, uint8_t regAddress, uint16_t regVal);
uint16_t fdc1004_read_register(I2C_HandleTypeDef i2c_handle, uint8_t regAddress);

HAL_StatusTypeDef fdc1004_write_ConfigChannel_1(I2C_HandleTypeDef i2c_handle, uint16_t regVal);
HAL_StatusTypeDef fdc1004_write_ConfigChannel_2(I2C_HandleTypeDef i2c_handle, uint16_t regVal);
HAL_StatusTypeDef fdc1004_write_ConfigChannel_3(I2C_HandleTypeDef i2c_handle, uint16_t regVal);
HAL_StatusTypeDef fdc1004_write_ConfigChannel_4(I2C_HandleTypeDef i2c_handle, uint16_t regVal);

uint16_t fdc1004_read_ConfigChannel_1(I2C_HandleTypeDef i2c_handle);
uint16_t fdc1004_read_ConfigChannel_2(I2C_HandleTypeDef i2c_handle);
uint16_t fdc1004_read_ConfigChannel_3(I2C_HandleTypeDef i2c_handle);
uint16_t fdc1004_read_ConfigChannel_4(I2C_HandleTypeDef i2c_handle);

uint16_t fdc1004_gen_SE_CONF_MEASx_value(uint8_t CHA_val, uint8_t CAPDAC_val, uint8_t CAPDAC_connected);
uint16_t fdc1004_gen_DIF_CONF_MEASx_value(uint8_t CHA_val, uint8_t CHB_val);

HAL_StatusTypeDef fdc1004_softReset(I2C_HandleTypeDef i2c_handle);
uint8_t fdc1004_softresetComplete(I2C_HandleTypeDef i2c_handle);

HAL_StatusTypeDef fdc1004_write_FDCreg(I2C_HandleTypeDef i2c_handle, uint16_t regVal);
uint16_t fdc1004_read_FDCreg(I2C_HandleTypeDef i2c_handle);
uint16_t fdc1004_gen_FDC_value(uint8_t CHANNEL_TRIG_val, uint8_t GLOBAL_RATE_val, uint8_t REPEAT_val);
uint8_t fdc1004_checkMeasComplete(I2C_HandleTypeDef i2c_handle, uint8_t relevantChannels_val);

HAL_StatusTypeDef fdc1004_write_OFFSET_CAL1(I2C_HandleTypeDef i2c_handle, uint16_t regVal);
HAL_StatusTypeDef fdc1004_write_OFFSET_CAL2(I2C_HandleTypeDef i2c_handle, uint16_t regVal);
HAL_StatusTypeDef fdc1004_write_OFFSET_CAL3(I2C_HandleTypeDef i2c_handle, uint16_t regVal);
HAL_StatusTypeDef fdc1004_write_OFFSET_CAL4(I2C_HandleTypeDef i2c_handle, uint16_t regVal);

HAL_StatusTypeDef fdc1004_write_GAIN_CAL1(I2C_HandleTypeDef i2c_handle, uint16_t regVal);
HAL_StatusTypeDef fdc1004_write_GAIN_CAL2(I2C_HandleTypeDef i2c_handle, uint16_t regVal);
HAL_StatusTypeDef fdc1004_write_GAIN_CAL3(I2C_HandleTypeDef i2c_handle, uint16_t regVal);
HAL_StatusTypeDef fdc1004_write_GAIN_CAL4(I2C_HandleTypeDef i2c_handle, uint16_t regVal);

void fdc1004_read_ManuID(I2C_HandleTypeDef i2c_handle);
void fdc1004_read_DevID(I2C_HandleTypeDef i2c_handle);

float fdc1004_calc_phys_CapValue_float(uint32_t raw_CapVal, float CAPDAC_phys_val);
float fdc1004_calc_CAPDAC_offset(uint8_t CAPDAC_val);
uint32_t fdc1004_read_RawCap_Value(I2C_HandleTypeDef i2c_handle, uint8_t MSB_pointer_address, uint8_t LSB_pointer_address);
uint32_t fdc1004_read_RawCapValue_Channel_1(I2C_HandleTypeDef i2c_handle);
uint32_t fdc1004_read_RawCapValue_Channel_2(I2C_HandleTypeDef i2c_handle);
uint32_t fdc1004_read_RawCapValue_Channel_3(I2C_HandleTypeDef i2c_handle);
uint32_t fdc1004_read_RawCapValue_Channel_4(I2C_HandleTypeDef i2c_handle);

int32_t fdc1004_calc_24bitTwoComplement2Int32(uint32_t raw_TwoCompVal);

void fdc1004_init(I2C_HandleTypeDef i2c_handle);
void fdc1004_setup_SE_Meas_Ch1(I2C_HandleTypeDef i2c_handle, uint8_t in_CAPDAC_val_ch1, uint8_t CAPDAC_connected_ch1);
void fdc1004_setup_SE_Meas_Ch2(I2C_HandleTypeDef i2c_handle, uint8_t in_CAPDAC_val_ch2, uint8_t CAPDAC_connected_ch2);
void fdc1004_trigger_Meas(I2C_HandleTypeDef i2c_handle, uint8_t sample_rate, uint8_t channel_trg_ID, uint8_t repeatMode_On);
float fdc1004_readMeasVal_Ch1(I2C_HandleTypeDef i2c_handle, uint8_t in_CAPDAC_val);
float fdc1004_readMeasVal_Ch2(I2C_HandleTypeDef i2c_handle, uint8_t in_CAPDAC_val);
void fdc1004_doneRequest_SE_Meas(I2C_HandleTypeDef i2c_handle, uint8_t channel_trg_ID);

float fdc1004_readMeasVal_channel_autoCapdac(I2C_HandleTypeDef i2c_handle, int channel);
float fdc1004_readMeasVal_channel_2_autoCapdac(I2C_HandleTypeDef i2c_handle);

#endif /* fdc1004_adc_h */
