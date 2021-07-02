#include "main.h"
#include "stm32f4xx_hal.h"
#include "ff.h"

FRESULT open_append (FIL* fp, const char* path); // Filename. If file exist it will be oppened. If not it will be created
uint8_t does_file_exist (
    FIL* fp,
    const char* path // Filename. If file exist it return true
);