#include "stm32f4xx_hal.h"
#include "utility_umweltspaeher.h"

void writeRegister_adc_MCP3903e(SPI_HandleTypeDef hspi_adc, uint8_t reg, unsigned long data);
uint32_t readRegister_adc_MCP3903e(SPI_HandleTypeDef hspi_adc,  uint8_t reg);
void init_adc_MCP3903e(SPI_HandleTypeDef hspi_adc, uint8_t osr);
void testADC(SPI_HandleTypeDef hspi_adc);
void setGain(SPI_HandleTypeDef hspi_adc, uint8_t channel, uint8_t gain, uint8_t boost);
void init_adc_continuousReading_config(SPI_HandleTypeDef hspi_adc);
int readoutADC_continuously(SPI_HandleTypeDef hspi_adc, uint8_t *adc_values);


static const uint8_t DEVICE_ADDR = 0x40;

static const uint8_t REG_CONFIG = 0x0A;

static const uint8_t REG_STATUS_COMM = 0x09; 

// See Datasheet (http://ww1.microchip.com/downloads/en/DeviceDoc/25048B.pdf)
// Page 25 TABLE 5-2
static const uint8_t OSR_32 = 0x0; // 17 Bits
static const uint8_t OSR_64 = 0x1; // 20 Bits
static const uint8_t OSR_128 = 0x2; // 23 Bits
static const uint8_t OSR_256 = 0x3; // 24 Bits

static const uint8_t REG_GAIN = 0x08;

static const uint8_t GAIN_1 = 0x0;
static const uint8_t GAIN_2 = 0x1;
static const uint8_t GAIN_4 = 0x2;
static const uint8_t GAIN_8 = 0x3;
static const uint8_t GAIN_16 = 0x4;
static const uint8_t GAIN_32 = 0x5;
