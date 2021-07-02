
//hx711_scale.h file
#ifndef hx711_scale_h
#define hx711_scale_h

#include "stm32f4xx_hal.h"

typedef struct _hx711Scale {
	int isWorking;
	GPIO_TypeDef* gpioSck;
	uint16_t pinSck;
	GPIO_TypeDef* gpioData;
	uint16_t pinData;
	int offset;
	int gain;
	// 1: channel A, gain factor 128
	// 2: channel B, gain factor 32
  // 3: channel A, gain factor 64
} HX711SCALE;


HX711SCALE newScale(GPIO_TypeDef* gpioSck, uint16_t pinSck, GPIO_TypeDef* gpioData,	uint16_t pinData, int gain);
void freeScale(HX711SCALE scale);
void initScale(HX711SCALE scales);
int scaleValue(HX711SCALE scale);

#endif /* hx711_scale_h */
