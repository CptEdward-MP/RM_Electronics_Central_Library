/*
 * imu_supervisor.h
 *
 *  Created on: Jan 27, 2026
 *      Author: Manansh Pandey
 */

#ifndef IMU_SUPERVISOR_H
#define IMU_SUPERVISOR_H

#include "FreeRTOS.h"
#include "task.h"

/* Supervisor task entry */
void imu_supervisor_task(void *arg);

#endif
