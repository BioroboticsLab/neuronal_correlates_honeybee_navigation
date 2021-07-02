
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F4xx_LP_MODES_H
#define __STM32F4xx_LP_MODES_H

#include "main.h"

void sleepMode(void);
void stopMode(void);
void standbyMode(void);
void standbyRTCMode(uint32_t seconds);
void standbyRTCBKPSRAMMode(void);

#endif /* __STM32F4xx_LP_MODES_H */
