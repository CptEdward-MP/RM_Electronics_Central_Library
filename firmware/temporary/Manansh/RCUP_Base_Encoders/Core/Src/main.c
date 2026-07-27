/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "usbd_cdc_if.h"
#include "usbd_cdc.h"
#include "string.h"
#include "math.h"   // for DEG2RAD
#include <stdlib.h>

#include "bno055.h"     // <--- ADD THIS
#include "cJSON.h"      // <--- ADD THIS
#include "stdio.h"
#include "usbd_cdc_if.h"
#include "bno055.h"
#include "bno055_stm32.h"
#include "usbd_cdc.h"
#include "string.h"
#include "math.h"   // for DEG2RAD
#include <stdlib.h>

#include "cJSON.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ONE_PERIOD 65536
#define HALF_PERIOD 32768
#define NUM_ENCODERS 3   // Number of encoders

#define DEG2RAD 0.017453292519943295  /* pi / 180 */


// Motor direction pins
#define M1   GPIO_PIN_4 //PB6 PWM
#define PORT_1 GPIOB

#define M2   GPIO_PIN_5//PB7 PWM
#define PORT_2 GPIOB

#define M3   GPIO_PIN_5//PA2 PWM
#define PORT_3 GPIOA

#define M4   GPIO_PIN_4//PB9 PWM
#define PORT_4 GPIOA


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
extern uint8_t usb_rx_buffer[128];
extern uint32_t usb_rx_length;
extern uint8_t usb_rx_flag;

#define MAGIC_KEY 0x12345678

char STM_Address[32] = "STM_ENCODER.PWM";   // your string



int pwm[4] = {0, 0, 0, 0};  // hardcoded PWM values
int received_array[4] = {69};

volatile int loop_time_ms = 0 ;

uint16_t tx_buf[3];

int32_t rx_buf[4];   // 4×4 = 16 bytes
volatile uint8_t rx_complete = 0;

int32_t encoder[3] = {2,3,9};
volatile bno055_vector_t g_euler;      // yaw/roll/pitch
volatile bno055_vector_t g_quat;       // w/x/y/z
volatile bno055_vector_t g_linaccel;   // p/q/r  (x/y/z)
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c2;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim5;

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;

/* USER CODE BEGIN PV */
uint8_t buffer[100];
char message[1024];
char mess[100];

//volatile bno055_vector_t g_euler;      // yaw/roll/pitch
//volatile bno055_vector_t g_quat;       // w/x/y/z
//volatile bno055_vector_t g_linaccel;   // p/q/r  (x/y/z)


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_I2C2_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM4_Init(void);
static void MX_TIM5_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int _write(int file, char *ptr, int len) {
	int DataIdx;
	for (DataIdx = 0; DataIdx < len; DataIdx++) {
		ITM_SendChar(*ptr++);
	}
	return len;
}

// Define constants for period wrapping
#define ONE_PERIOD 65536
#define HALF_PERIOD 32768
#define NUM_ENCODERS 3   // Number of encoders

typedef struct {
	volatile uint16_t timer_count;   // Current timer count (from hardware)
	volatile int32_t prev_unwrapped; // Previous unwrapped count
	volatile int32_t unwrapped_count; // Accumulated unwrapped count
} Encoder;

// Initialize an array of encoders
Encoder encoders[NUM_ENCODERS];

void unwrap_encoder(Encoder *encoder) {
	// Get the current 16-bit count and previous unwrapped count
	uint16_t current_count = encoder->timer_count;
	int32_t *prev = &(encoder->prev_unwrapped);

	int32_t c32 = (int32_t) current_count - HALF_PERIOD; // Remove half period to determine sign of the wrap
	int32_t dif = (c32 - *prev);              // Calculate the difference

	// Wrap difference from -HALF_PERIOD to HALF_PERIOD
	int32_t mod_dif = ((dif + HALF_PERIOD) % ONE_PERIOD) - HALF_PERIOD;
	if (dif < -HALF_PERIOD) {
		mod_dif += ONE_PERIOD; // Account for mod of negative number behavior in C
	}

	// Update unwrapped count and store it in the encoder structure
	int32_t unwrapped = *prev + mod_dif;
	encoder->unwrapped_count = unwrapped + HALF_PERIOD; // Store unwrapped count
	*prev = unwrapped;  // Update previous unwrapped count
}

void update_encoders(void) {
	// Read current counts from hardware timers (assuming you are using TIM1, TIM2, TIM3, and TIM4)
	encoders[2].timer_count = (TIM3->CNT);
	encoders[0].timer_count = (TIM1->CNT);
	encoders[1].timer_count = (TIM2->CNT);
	//encoders[3].timer_count = (TIM4->CNT);

	// Update the unwrapped count for each encoder
	for (int i = 0; i < NUM_ENCODERS; i++){
		unwrap_encoder(&encoders[i]);  // Call unwrap_encoder for each encoder


	}
}




/**
 * @brief Set speed and direction of 4 amotors
 * @param motor_speeds: array of 4 int8_t values (-100 to 100)
 */

void setMotors(int motor_speeds[4])
{
    // Safety limit
    for(int i = 0; i < 4; i++)
    {
        if (motor_speeds[i] > 80 || motor_speeds[i] < -80)
        {
            motor_speeds[0] = 0;
            motor_speeds[1] = 0;
            motor_speeds[2] = 0;
            motor_speeds[3] = 0;
            break;
        }
    }

    int duty;
    int dir;

    // Motor 1
    duty = abs(motor_speeds[0]) * 10;
    dir  = motor_speeds[0] > 0 ? 1 : 0;
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, duty);
    HAL_GPIO_WritePin(PORT_1, M1, dir);

    // Motor 2
    duty = abs(motor_speeds[1]) * 10;
    dir  = motor_speeds[1] > 0 ? 1 : 0;
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, duty);
    HAL_GPIO_WritePin(PORT_2, M2, dir);

    // Motor 3
    duty = abs(motor_speeds[2]) * 10;
    dir  = motor_speeds[2] > 0 ? 1 : 0;
    __HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_3, duty);
    HAL_GPIO_WritePin(PORT_3, M3, dir);

    // Motor 4
    duty = abs(motor_speeds[3]) * 10;
    dir  = motor_speeds[3] > 0 ? 1 : 0;
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, duty);
    HAL_GPIO_WritePin(PORT_4, M4, dir);
}

//void setMotors(int motor_speeds[4])
//{
//	//software limit for motor speed value
//	for(int i = 0 ; i <4 ; i++){
//		if(motor_speeds[i]>80)  {motor_speeds[i] = 0;}
//	}
//
//
//
//	int p[4];
//
//	p[0] = abs(motor_speeds[0])*10;
//	int dir1 = motor_speeds[0]>0 ? 1 : 0 ;
//	__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1 ,p[0] );
//	HAL_GPIO_WritePin(PORT_1, M1, dir1);
//
//	p[1] = abs(motor_speeds[1])*10;
//	int dir2 = motor_speeds[1]>0 ? 1 : 0 ;
//	__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2 ,p[1] );
//	HAL_GPIO_WritePin(PORT_2, M2, dir2);
//
//
//	p[2] = abs(motor_speeds[2])*10;
//	int dir3 = motor_speeds[2]>0 ? 1 : 0 ;
//	__HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_3 ,p[2] );
//	HAL_GPIO_WritePin(PORT_3, M3, dir3);
//
//	p[3] = abs(motor_speeds[3])*10;
//	int dir4 = motor_speeds[3]>0 ? 1 : 0 ;
//	__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4 ,p[3] );
//	HAL_GPIO_WritePin(PORT_4, M4, dir4);
//
//
//
//
////	__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1 , motor_speeds[0] );
////	__HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_3 , motor_speeds[1] );
////	__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2 , motor_speeds[2] );
////	__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4 , motor_speeds[3] );
//}

//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
//{
//    if (huart->Instance == USART1)
//    {
//        rx_complete = 1;
//
//        // VERY IMPORTANT — restart DMA correctly
//        HAL_UART_Receive_DMA(&huart1, (uint8_t*)rx_buf, sizeof(rx_buf));
//    }
//}

//
//
void parse_json_pwm(const char *json_str)

{
	 cJSON *root = cJSON_Parse(json_str);
	    if(!root) {
	        const char *err = "Invalid JSON\n";
	        CDC_Transmit_FS((uint8_t*)err, strlen(err));
	        return;
	    }

	    const char *keys[] = {"pwm1","pwm2","pwm3","pwm4"};
	    char msg[64];

	    for(int i=0;i<4;i++){
	        cJSON *item = cJSON_GetObjectItem(root, keys[i]);
	        if(cJSON_IsNumber(item)){
	            pwm[i] = item->valueint;
	        }
	    }

	    sprintf(msg,"PWM: %d %d %d %d\n", pwm[0], pwm[1], pwm[2], pwm[3]);
	    CDC_Transmit_FS((uint8_t*)msg, strlen(msg));

	    cJSON_Delete(root);
}


//void send_Encoder(void){
//
//	int32_t values[3];
//	values[0] = encoders[0].unwrapped_count;
//	values[1] = encoders[1].unwrapped_count;
//	values[2] =encoders[2].unwrapped_count;
//
//
//	HAL_UART_Transmit(&huart1, (uint8_t*)values, sizeof(values), HAL_MAX_DELAY);
//
//}

void send_Encoder(void)
{
    int32_t values[3];

    values[0] = encoders[0].unwrapped_count;
    values[1] = encoders[1].unwrapped_count;
    values[2] = encoders[2].unwrapped_count;


    HAL_UART_Transmit(&huart1, (uint8_t*)values, sizeof(values), HAL_MAX_DELAY);
}
//
//void send_encoder_packet(void)
//{
//    int32_t packet[4];
//    packet[0] = MAGIC_KEY;
//    packet[1] = encoders[0].unwrapped_count;
//    packet[2] = encoders[1].unwrapped_count;
//    packet[3] = encoders[2].unwrapped_count;
//
//    HAL_UART_Transmit(&huart1, (uint8_t*)packet, sizeof(packet), HAL_MAX_DELAY);
//}
//void send_encoders_blocking(void)
//{
//    int32_t packet[3];
//    packet[0] = encoders[0].unwrapped_count;
//    packet[1] = encoders[1].unwrapped_count;
//    packet[2] = encoders[2].unwrapped_count;
//
//    HAL_UART_Transmit(&huart1, (uint8_t*)packet, sizeof(packet), HAL_MAX_DELAY);
//}

//void send_Encoder(void){
//
//	int32_t values[3];
//
//	for(int i = 0 ; i<3 ; i++){
//		values[i] = encoders[i].unwrapped_count;
//	}
//
//	HAL_UART_Transmit(&huart1, (uint8_t*)values, sizeof(values), HAL_MAX_DELAY);
//
//}




/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_USB_DEVICE_Init();
  MX_I2C2_Init();
  MX_USART1_UART_Init();
  MX_TIM4_Init();
  MX_TIM5_Init();
  /* USER CODE BEGIN 2 */


  // Start PWM channels once at init
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);

  // Control motors


	bno055_assignI2C(&hi2c2);
	bno055_setup();
	bno055_enableExternalCrystal();
	bno055_setOperationModeNDOF();
	HAL_Delay(10);

	HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);
	HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL);
	HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);

	// HAL_UART_Receive_DMA(&huart1, (uint8_t*)rx_buf, sizeof(rx_buf));



  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {



//
//	  __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, pwm[1]);
//
    setMotors(pwm);
    /* Take timestamp at measurement moment */
    update_encoders();

    uint32_t t_now = HAL_GetTick();

    /* Build message */
    sprintf(message,
        "E{"
        "\"t\":%lu,"
        "\"a\":%ld,"
        "\"b\":%ld,"
        "\"c\":%ld"
        "}\r\n",
        t_now,
        encoders[0].unwrapped_count,
        encoders[1].unwrapped_count,
        encoders[2].unwrapped_count
    );
    //send_Encoder();
//
//



//	  if (rx_complete)
//	  {
//	      rx_complete = 0;
//
//	      pwm[0] = rx_buf[0];
//	      pwm[1] = rx_buf[1];
//	      pwm[2] = rx_buf[2];
//	      pwm[3] = rx_buf[3];
//
////	      char msg[64];
////	      sprintf(msg, "RX PWM: %u %u %u %u\r\n",
////	              pwm[0], pwm[1], pwm[2], pwm[3]);
////
////	      CDC_Transmit_FS((uint8_t*)msg, strlen(msg));
//	  }


   // send_encoder_values_Uart();
//		sprintf(mess, "{\"en1\": %ld, \"en2\": %ld, \"en3\": %ld}\r\n",
//		        encoders[0].unwrapped_count,
//		        encoders[1].unwrapped_count,
//		        encoders[2].unwrapped_count);
//		CDC_Transmit_FS(mess, strlen(mess));

        /* read IMU vectors */
//        bno055_vector_t v_euler = bno055_getVectorEuler();        // v_euler.x = yaw (deg), v_euler.y = roll (deg), v_euler.z = pitch (deg)
//        bno055_vector_t v_quat  = bno055_getVectorQuaternion();   // v_quat.w, v_quat.x, v_quat.y, v_quat.z  (library stores w,x,y,z)
//        bno055_vector_t v_lin   = bno055_getVectorLinearAccel();  // linear accel in m/s^2
//        bno055_vector_t v_gyro  = bno055_getVectorGyroscope();   // gyro in deg/s (x=roll rate, y=pitch rate, z=yaw rate)
//
//
//        /* convert gyro to rad/s */
//        double gyro_x_rad = ((double)v_gyro.x) * DEG2RAD;   // angular_velocity.x (rad/s)
//        double gyro_y_rad = ((double)v_gyro.y) * DEG2RAD;   // angular_velocity.y (rad/s)
//        double gyro_z_rad = ((double)v_gyro.z) * DEG2RAD;   // angular_velocity.z (rad/s)

        /* Build JSON:
           encoders a-d,
           orientation quaternion (x,y,z,w) -> sensor_msgs/Imu order,
           angular_velocity (x,y,z) in rad/s,
           linear_acceleration (x,y,z) in m/s^2,
           debug yaw/roll/pitch in degrees
        */
//        sprintf(message,
//                "{"
//                "\"a\": %ld, \"b\": %ld, \"c\": %ld, "
//                "\"orientation_x\": %.6f, \"orientation_y\": %.6f, \"orientation_z\": %.6f, \"orientation_w\": %.6f, "
//                "\"angular_velocity_x\": %.6f, \"angular_velocity_y\": %.6f, \"angular_velocity_z\": %.6f, "
//                "\"linear_acceleration_x\": %.6f, \"linear_acceleration_y\": %.6f, \"linear_acceleration_z\": %.6f, "
//                "\"yaw_deg\": %.2f, \"roll_deg\": %.2f, \"pitch_deg\": %.2f"
//                "}\r\n",
//                encoders[0].unwrapped_count, encoders[1].unwrapped_count,
//                encoders[2].unwrapped_count,
//                /* quaternion -> use x,y,z,w order for ROS */
//                (double)v_quat.x, (double)v_quat.y, (double)v_quat.z, (double)v_quat.w,
//                /* gyro in rad/s */
//                gyro_x_rad, gyro_y_rad, gyro_z_rad,
//                /* linear accel */
//                (double)v_lin.x, (double)v_lin.y, (double)v_lin.z,
//                /* debug Euler (library order: yaw, roll, pitch in degrees) */
//                (double)v_euler.x, (double)v_euler.y, (double)v_euler.z
//            );
//        sprintf(message,
//            "E{"
//            "\"a\": %ld, \"b\": %ld, \"c\": %ld"
//            "}\r\n",
//            encoders[0].unwrapped_count,
//            encoders[1].unwrapped_count,
//            encoders[2].unwrapped_count
//        );
//
////        }
//        int t_now = HAL_GetTick();

//
//        sprintf(message,
//            "{"
//            "\"Name\": \"%s\", "                                  // <--- NEW FIRST FIELD
//            "\"time_ms\": %lu, "
//            "\"a\": %ld, \"b\": %ld, \"c\": %ld, "
//            "\"orientation_x\": %.6f, \"orientation_y\": %.6f, \"orientation_z\": %.6f, \"orientation_w\": %.6f, "
//            "\"angular_velocity_x\": %.6f, \"angular_velocity_y\": %.6f, \"angular_velocity_z\": %.6f, "
//            "\"linear_acceleration_x\": %.6f, \"linear_acceleration_y\": %.6f, \"linear_acceleration_z\": %.6f, "
//            "\"yaw_deg\": %.2f, \"roll_deg\": %.2f, \"pitch_deg\": %.2f"
//            "}\r\n",
//
//			STM_Address,                 // <--- FIRST ARGUMENT (STRING)
//            t_now,
//            encoders[0].unwrapped_count, encoders[1].unwrapped_count, encoders[2].unwrapped_count,
//            (double)v_quat.x, (double)v_quat.y, (double)v_quat.z, (double)v_quat.w,
//            gyro_x_rad, gyro_y_rad, gyro_z_rad,
//            (double)v_lin.x, (double)v_lin.y, (double)v_lin.z,
//            (double)v_euler.x, (double)v_euler.y, (double)v_euler.z
//        );

//
//
//
//
//        t_now = HAL_GetTick();   // returns ms
		while (CDC_Transmit_FS((uint8_t*)message, strlen(message)) == USBD_BUSY) {
		      HAL_Delay(1);
		    }


//
//
        if (usb_rx_flag)
        {
            usb_rx_flag = 0;
            usb_rx_buffer[usb_rx_length] = '\0';

           parse_json_pwm((char*)usb_rx_buffer);

            // Example: send acknowledgment back
//            char reply[64];
//            sprintf(reply, "Got: %d,%d,%d,%d\r\n",
//                    pwm[0], pwm[1],
//                    pwm[2], pwm[3]);
//            CDC_Transmit_FS((uint8_t*)reply, strlen(reply));
        }

  }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 100000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 5;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 5;
  if (HAL_TIM_Encoder_Init(&htim1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 65535;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 5;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 0;
  if (HAL_TIM_Encoder_Init(&htim2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 5;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 5;
  if (HAL_TIM_Encoder_Init(&htim3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 5-1;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 1000;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */
  HAL_TIM_MspPostInit(&htim4);

}

/**
  * @brief TIM5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM5_Init(void)
{

  /* USER CODE BEGIN TIM5_Init 0 */

  /* USER CODE END TIM5_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM5_Init 1 */

  /* USER CODE END TIM5_Init 1 */
  htim5.Instance = TIM5;
  htim5.Init.Prescaler = 5-1;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = 1000;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim5) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim5, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM5_Init 2 */

  /* USER CODE END TIM5_Init 2 */
  HAL_TIM_MspPostInit(&htim5);

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */
  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4|GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4|GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA4 PA5 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB4 PB5 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */
  /* TIM4 PWM Pins Configuration:
     PA6 -> TIM4_CH1
     PA7 -> TIM4_CH2
     PB8 -> TIM4_CH3
     PB9 -> TIM4_CH4
  */
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

  // --- Channel 1 and 2 on Port A ---
  GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
  GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // --- Channel 3 and 4 on Port B ---
  GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
  GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
