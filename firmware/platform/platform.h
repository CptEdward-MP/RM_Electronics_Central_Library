#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>

void Platform_Write(const char *data);

float Platform_ADC_Read(
    unsigned char channel);

void Platform_ADC_SetValue(
    unsigned char channel,
    float value);

uint32_t Platform_Time_GetMs(void);

#endif

