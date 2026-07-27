/*
 * app_task.h
 *
 *  Created on: Jan 18, 2026
 *      Author: Manansh Pandey
 */

#ifndef APP_TASKS_H
#define APP_TASKS_H

#include <stdint.h>
#include "FreeRTOS.h"
#include "queue.h"

/* ================= IMU STATE ================= */

//typedef enum {
//    IMU_STATE_INIT = 0,
//    IMU_STATE_READY,
//    IMU_STATE_ERROR
//} imu_state_t;

/* ================= IMU DATA ================= */


/* ================= TASK APIS ================= */

/* Task entry points */
void imu_task(void *arg);
void usb_task(void *arg);

/* Init function */
void app_tasks_init(void);

#endif /* APP_TASKS_H */
