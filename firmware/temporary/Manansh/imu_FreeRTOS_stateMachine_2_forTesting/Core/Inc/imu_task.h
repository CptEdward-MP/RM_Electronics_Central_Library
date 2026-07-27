/*
 * imu_task.h
 *
 *  Created on: Jan 21, 2026
 *      Author: Manansh Pandey
 */

#ifndef IMU_TASK_H
#define IMU_TASK_H

#include <stdint.h>
#include "FreeRTOS.h"
#include "queue.h"


/* ================= API ================= */

void imu_task(void *arg);

extern volatile imu_state_t imu_state;
extern QueueHandle_t imu_data_queue;
#endif
