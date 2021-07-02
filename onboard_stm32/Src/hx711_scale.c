
// hx711_scale.c file
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "hx711_scale.h"
#include "stm32f4xx_hal.h"
#include "utility_umweltspaeher.h"

extern UART_HandleTypeDef huart2;

// Constructor for HX711
HX711SCALE newScale(GPIO_TypeDef* gpioSck, uint16_t pinSck, GPIO_TypeDef* gpioData,	uint16_t pinData, int gain) { 
  HX711SCALE* hx711Scale = malloc(sizeof(HX711SCALE));
  hx711Scale->gpioSck = gpioSck;
  hx711Scale->gpioData = gpioData;
	hx711Scale->pinSck = pinSck;
	hx711Scale->pinData = pinData;
	hx711Scale->gain = gain;
  return *hx711Scale;
}

void freeScale(HX711SCALE scale) {
		free(&scale);
}

void initScale(HX711SCALE scale) {
	// loop through array of scale structs
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = scale.pinSck;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(scale.gpioSck, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = scale.pinData;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(scale.gpioData, &GPIO_InitStruct);

	HAL_GPIO_WritePin(scale.gpioData, scale.pinSck, GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(scale.gpioSck, scale.pinSck, GPIO_PIN_SET);
	HAL_Delay(70);
	HAL_GPIO_WritePin(scale.gpioData, scale.pinSck, GPIO_PIN_RESET);
}

int scaleValue(HX711SCALE scale) {
	scale.isWorking = 1;
	int retry = 0;
	while (HAL_GPIO_ReadPin(scale.gpioData, scale.pinData) == 1) {
		// if all scales are reading 0, then either they are all broken
		// or the number of retries needs to be increased
		if (retry > 2000000) {
			scale.isWorking = 0;
			break;
		}
		retry++;
	}
	// if scale is working then retrieve value via GPIO
	int value = 0;
	if (scale.isWorking) {
		int buffer = 0;
		for (uint8_t j = 0; j < 24; ++j) {
			HAL_GPIO_WritePin(scale.gpioSck, scale.pinSck, GPIO_PIN_SET);
			buffer = buffer << 1 ;
			if (HAL_GPIO_ReadPin(scale.gpioData, scale.pinData)) {
					buffer++;
			}
			HAL_GPIO_WritePin(scale.gpioSck, scale.pinSck, GPIO_PIN_RESET);
		}
		// set the gain, possible values are 1, 2, or 3. 
		// https://cdn.sparkfun.com/datasheets/Sensors/ForceFlex/hx711_english.pdf
		for (uint8_t j = 0; j < scale.gain; ++j) {
			HAL_GPIO_WritePin(scale.gpioSck, scale.pinSck, GPIO_PIN_SET); // High
			HAL_GPIO_WritePin(scale.gpioSck, scale.pinSck, GPIO_PIN_RESET); // Low
		}
		// add value to array
		value = buffer ^ 0x800000;
	}
	return value;
}
