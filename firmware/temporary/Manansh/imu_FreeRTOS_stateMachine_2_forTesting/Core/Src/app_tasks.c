#include <imu_logic.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "main.h"
#include "stm32f4xx_hal.h"

#include "usbd_cdc_if.h"
#include <string.h>
#include <stdint.h>

#include "imu_task.h"
#include "usb_task.h"
#include "imu_supervisor.h"

/* ================= INIT ================= */

void app_tasks_init(void)
{
    imu_data_queue = xQueueCreate(1, sizeof(imu_9val_scaled_t));

    xTaskCreate(imu_task, "IMU", 512, NULL, 3, NULL);        // HIGHEST
    xTaskCreate(imu_supervisor_task, "IMU_SUP", 512, NULL, 2, NULL); // Medium
    xTaskCreate(usb_task, "USB", 512, NULL, 1, NULL);        // Lowest
}

