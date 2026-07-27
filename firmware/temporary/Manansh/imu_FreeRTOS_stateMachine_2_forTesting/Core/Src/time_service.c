/*
 * time_service.c
 *
 *  Created on: Feb 22, 2026
 *      Author: Manansh Pandey
 */


#include "time_service.h"
#include "FreeRTOS.h"
#include "task.h"

/* ------------------------------------------------------------
 * Initialization (not strictly needed now, kept for future)
 * ------------------------------------------------------------ */
void time_init(void)
{
    /* Nothing required for FreeRTOS tick-based implementation */
}

/* ------------------------------------------------------------
 * Return current system time in milliseconds
 * ------------------------------------------------------------ */
uint32_t time_now_ms(void)
{
    return (uint32_t)xTaskGetTickCount();
}

/* ------------------------------------------------------------
 * Non-blocking expiration check
 * Safe for tick overflow
 * ------------------------------------------------------------ */
bool time_expired(uint32_t start_time, uint32_t duration_ms)
{
    return ((uint32_t)(xTaskGetTickCount() - start_time) >= duration_ms);
}

/* ------------------------------------------------------------
 * Optional blocking sleep wrapper
 * ------------------------------------------------------------ */
void Delay(uint32_t duration_ms)
{
    vTaskDelay(pdMS_TO_TICKS(duration_ms));
}
