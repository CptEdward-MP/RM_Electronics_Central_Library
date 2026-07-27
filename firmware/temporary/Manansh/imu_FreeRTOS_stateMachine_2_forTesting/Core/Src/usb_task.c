/*
 * usb_task.c
 */

#include "imu_logic.h"
#include "main.h"
#include "stm32f4xx_hal.h"

#include "usbd_cdc_if.h"
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "imu_task.h"   // for imu_data_queue
#include "board_init.h" // ⭐ IMPORTANT (getter)

#include "cJSON.h"

/* ============================================================
 * USB TASK
 * ============================================================ */

void usb_task(void *arg)
{
    imu_t *imu = imu_get();   // ⭐ FIX: get instance

    imu_9val_scaled_t rx;
    imu_9val_scaled_t last_rx = {0};
    bool has_valid_sample = false;

    char msg[256];
    TickType_t lastWake = xTaskGetTickCount();

    for (;;)
    {
        /* --------------------------------------------------
         * 1) Drain queue → keep freshest IMU data
         * -------------------------------------------------- */
        while (xQueueReceive(imu_data_queue, &rx, 0) == pdPASS)
        {
            last_rx = rx;
            has_valid_sample = true;
        }

        /* --------------------------------------------------
         * 2) Read IMU state (FIXED)
         * -------------------------------------------------- */
        imu_state_t st = imu_state_get(imu);

        /* --------------------------------------------------
         * 3) Build JSON packet
         * -------------------------------------------------- */
        snprintf(msg, sizeof(msg),
            "{"
            "\"state\":%d,"
            "\"cal\":%u,"
            "\"yaw\":%.2f"
            "}\r\n",

            st,
            has_valid_sample ? last_rx.calibrated : 0,
            has_valid_sample ? last_rx.yaw : 0.0f
        );

        /* --------------------------------------------------
         * 4) Non-blocking transmit
         * -------------------------------------------------- */
        CDC_Transmit_FS((uint8_t *)msg, strlen(msg));

        /* --------------------------------------------------
         * 5) Periodic timing
         * -------------------------------------------------- */
        vTaskDelayUntil(&lastWake, pdMS_TO_TICKS(10));
    }
}
