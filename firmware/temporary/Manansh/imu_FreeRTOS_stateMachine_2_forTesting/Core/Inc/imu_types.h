/*
 * imu_types.h
 *
 *  Created on: Feb 3, 2026
 *      Author: Manansh Pandey
 */

#ifndef INC_IMU_TYPES_H_
#define INC_IMU_TYPES_H_

#include <stdint.h>

/* Shared between Driver and HW layer */
typedef struct
{
    int16_t yaw;
    int16_t roll;
    int16_t pitch;
} imu_euler_t;

#endif /* INC_IMU_TYPES_H_ */
