/*
 * time_service.h
 *
 *  Created on: Feb 22, 2026
 *      Author: Manansh Pandey
 */

#ifndef INC_TIME_SERVICE_H_
#define INC_TIME_SERVICE_H_

#ifndef TIME_SERVICE_H
#define TIME_SERVICE_H

#include <stdint.h>
#include <stdbool.h>

/* Initialize time service (optional for now) */
void time_init(void);

/* Get current time in milliseconds */
uint32_t time_now_ms(void);

/* Check if duration has expired (non-blocking) */
bool time_expired(uint32_t start_time, uint32_t duration_ms);

/* Optional blocking sleep (wrapper around RTOS delay) */
void Delay(uint32_t duration_ms);

#endif

#endif /* INC_TIME_SERVICE_H_ */
