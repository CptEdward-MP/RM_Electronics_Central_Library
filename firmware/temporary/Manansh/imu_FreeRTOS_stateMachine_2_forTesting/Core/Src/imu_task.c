/*
 * imu_task.c
 *
 * Application Layer - IMU RTOS Task
 */

#include "imu_logic.h"
#include "imu_hw.h"
#include "bno055.h"

#include "main.h"
#include "stm32f4xx_hal.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "board_init.h"

/* ============================================================
 * SYNC GPIO (Optional)
 * ============================================================ */

#define IMU_SYNC_GPIO_Port   GPIOB
#define IMU_SYNC_GPIO_Pin    GPIO_PIN_4

/* ============================================================
 * Queue Handle
 * ============================================================ */

QueueHandle_t imu_data_queue = NULL;

/* ============================================================
 * IMU INSTANCES (NEW)
 * ============================================================ */

//static hw_i2c_t i2c_bus;
//static bno055_t bno;


/* ============================================================
 * IMU Task
 * ============================================================ */
void imu_task(void *arg)
{
    imu_system_init();

    imu_t *imu = imu_get();   // ✅ GET INSTANCE

    imu_9val_scaled_t sample;

    TickType_t lastWake  = xTaskGetTickCount();
    TickType_t lastStart = lastWake;

    /* Create queue */
    imu_data_queue = xQueueCreate(1, sizeof(imu_9val_scaled_t));
    configASSERT(imu_data_queue);

    /* -------- Initialization Loop -------- */
    while (imu_init(imu) != IMU_STATUS_OK)
    {
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    /* -------- Periodic Execution -------- */
    for (;;)
    {
        TickType_t now = xTaskGetTickCount();
        sample.period_jitter_ms =
            (now - lastStart) * portTICK_PERIOD_MS;
        lastStart = now;

        uint32_t t1 = DWT->CYCCNT;

        imu_poll(imu);
        imu_status_t st = imu_read(imu, &sample);

        uint32_t t2 = DWT->CYCCNT;

        sample.imu_us =
            (t2 - t1) / (SystemCoreClock / 1000000);

        sample.state = imu_state_get(imu);
        sample.calibrated = imu_is_calibrated(imu);

        if (st == IMU_STATUS_OK)
        {
            xQueueOverwrite(imu_data_queue, &sample);
        }

        vTaskDelayUntil(&lastWake, pdMS_TO_TICKS(300));
    }
}
