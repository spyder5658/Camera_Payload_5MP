/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  * @date           : 04-07-2025
  * @author         : Sarthak Chaudhary
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
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <inttypes.h>
#include "camera.h"
#include "watchdog.h"
#include "radio.h"
#include "mini_morse.h"

int _write(int file, char *data, int len) {
    HAL_UART_Transmit(&huart1, (uint8_t*)data, len, HAL_MAX_DELAY);
    return len;
}
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */


#define SLAVE_ADDR      (0x12 << 1 )  // Change to your slave address (7-bit shifted)
#define SSDV_PKT_SIZE   224
#define MAX_PACKETS     256         // Max packets to read (adjust if needed)

// uint8_t rx_buffer[SSDV_PKT_SIZE];

// -------------------- I2C Commands --------------------
#define CMD_CAPTURE         0x10
#define CMD_STREAM_SSDV     0x20
#define CMD_PRESTORED       0x90

// -------------------- Master functions --------------------

void send_command(uint8_t cmd)
{
    HAL_I2C_Master_Transmit(&hi2c2, SLAVE_ADDR, &cmd, 1, HAL_MAX_DELAY);
}

int read_ssdv_packet(uint8_t *packet)
{
    HAL_StatusTypeDef status;
    status = HAL_I2C_Master_Receive(&hi2c2, SLAVE_ADDR, packet, SSDV_PKT_SIZE, 100);

    if (status != HAL_OK) return 0;

    // Check for dummy packet indicating end of stream
    if (packet[0] == 0xFF) return 0;

    return 1;
}
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_I2C2_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  watchdog_init();
    if (!radio_init())
  {
    printf("Radio error\n");
    while (1);
  }
  else
  {
    printf("Radio success!\n");
  }

  printf("after setup\n");


    // printf("Master MCU initialized.\n");
    // camera_on();
    // HAL_Delay(3000);

    // // -------------------- Capture Image --------------------
    // printf("Sending capture command to slave...\n");
    // send_command(CMD_CAPTURE);

    // HAL_Delay(8000); // Wait for slave to capture & encode

    // // -------------------- Stream SSDV Packets --------------------
    // printf("Requesting SSDV stream from slave...\n");
    // send_command(CMD_STREAM_SSDV);
    // HAL_Delay(200);

    // int packet_count = 0;
    // while (packet_count < MAX_PACKETS)
    // {
    //     if (!read_ssdv_packet(rx_buffer)) break;

    //     printf("Packet %d:\n", packet_count + 1);
    //     for (int i = 0; i < SSDV_PKT_SIZE; i++)
    //     {
    //         printf("0x%02X, ", rx_buffer[i]);
    //         if ((i + 1) % 16 == 0) printf("\n");
    //     }
    //     printf("\n-----------------------------\n");

    //     packet_count++;
    //     HAL_Delay(5); // Optional: small delay between packets
    // }

    // printf("SSDV streaming complete. Total packets: %d\n", packet_count);
    // HAL_Delay(1000);
    // camera_off();


 

    //final camera capture flow;
    // /*-----------------------------------------------------*/
    // camera_on();
    // HAL_Delay(3000);
    // i2c_check();
    // HAL_Delay(1000);
    // // set_properties() // optional
    // camera_request_payload();
    // camera_off();

    // HAL_Delay(5000);

    // camera_on();
    // HAL_Delay(3000);
    // i2c_check();
    // HAL_Delay(1000);
    // request_prestored_image();
    // camera_off();

    /*-----------------------------------------------------*/

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

    //  HAL_Delay(1000);
    // camera_on();
    // HAL_Delay(3000);     //relaxation time for slave board to powerup and get configured ............
    // /* === Periodic I2C bus health check === */
    // if (HAL_GetTick() - last_i2c_check_time > 1000)  // every 1s
    // {
    //   last_i2c_check_time = HAL_GetTick();

    //   // Timeout: if no I2C activity in 3s, and BUSY flag still set
    //   if ((HAL_GetTick() - last_i2c_activity > 3000) &&
    //       (__HAL_I2C_GET_FLAG(&hi2c2, I2C_FLAG_BUSY)))
    //   {
    //     printf("I2C timeout detected! Resetting I2C2...\r\n");

    // //     // Reset I2C2 peripheral safely
    //     __HAL_RCC_I2C2_FORCE_RESET();
    //     HAL_Delay(2);
    //     __HAL_RCC_I2C2_RELEASE_RESET();
    //     MX_I2C2_Init();  // Reinitialize I2C2
       

    //   }
    // }
    // HAL_Delay(1000);
    //   set_brightness_(CAM_BRIGHTNESS_LEVEL_4);
    // set_effect_(CAM_COLOR_FX_BW );
    // set_contrast_(CAM_CONTRAST_LEVEL_MINUS_3 );
    // request_image_capture();
    // HAL_Delay(8000);  // Allow time for image capture & SSDV encoding

    // //   // Step 2: Request SSDV Stream
    // request_ssdv_stream();
    // HAL_Delay(200);

    // // Step 3: Read All SSDV Packets
    // read_ssdv_stream();
    // camera_off();
    // HAL_Delay(5000);
    // printf("capturing prestored image now!!\n");
    // if (HAL_GetTick() - last_i2c_check_time > 1000)  // every 1s
    // {
    //   last_i2c_check_time = HAL_GetTick();

    //   // Timeout: if no I2C activity in 3s, and BUSY flag still set
    //   if ((HAL_GetTick() - last_i2c_activity > 3000) &&
    //       (__HAL_I2C_GET_FLAG(&hi2c2, I2C_FLAG_BUSY)))
    //   {
    //     printf("I2C timeout detected! Resetting I2C2...\r\n");

    //     // Reset I2C2 peripheral safely
    //     __HAL_RCC_I2C2_FORCE_RESET();
    //     HAL_Delay(2);
    //     __HAL_RCC_I2C2_RELEASE_RESET();
    //     MX_I2C2_Init();  // Reinitialize I2C2
       

    //   }
    // }



    // printf("i2c recovery going on....\r\n");
    // printf("turining camera on\r\n");
    // camera_on();
    // HAL_Delay(3000);
    // i2c_check();
    // HAL_Delay(1000);
    // // set_properties() // optional
    // camera_request_payload();
    // // printf("turining camera off\r\n");
    // camera_off();

    // HAL_Delay(5000);












    // printf("turining camera on\r\n");
    // camera_on();
    // HAL_Delay(3000);
    // i2c_check();
    // HAL_Delay(1000);
    // request_prestored_image();
    // // printf("turining camera off\r\n");
    // camera_off();

    camera_on();
    HAL_Delay(3000);
    i2c_check();
    HAL_Delay(1000);
    // set_properties() // optional
    camera_request_payload();
    camera_off();

    HAL_Delay(5000);

    camera_on();
    HAL_Delay(3000);
    i2c_check();
    HAL_Delay(1000);
    request_prestored_image();
    camera_off();









    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
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

#ifdef  USE_FULL_ASSERT
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
