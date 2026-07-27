/*
 * usb_task.h
 *
 *  Created on: Jan 21, 2026
 *      Author: Manansh Pandey
 */

#ifndef INC_USB_TASK_H_
#define INC_USB_TASK_H_


extern QueueHandle_t imu_data_queue;


void usb_task(void *arg);


#endif /* INC_USB_TASK_H_ */
