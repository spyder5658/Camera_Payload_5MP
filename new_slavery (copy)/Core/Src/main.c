/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  * @date           : 04-07-2025
  * @author         : Sarthak Chaudhary
  *****************************************************************************
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
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <inttypes.h>
#include "camera.h"
#include "i2c_slave.h"
#include "watchdog.h"
int _write(int file, char *data, int len) {
    HAL_UART_Transmit(&huart1, (uint8_t*)data, len, HAL_MAX_DELAY);
    return len;
}
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
// Globals
extern volatile uint8_t capture_requested;
extern volatile uint8_t send_all_packets;
extern volatile uint8_t prestore_image_requested;
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
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
  // uint32_t last_i2c_check_time = 0;
  // extern uint32_t last_i2c_activity;  // Ensure declared in .c file

  // printf("slave got started..........\r\n");
  // if(HAL_I2C_EnableListen_IT(&hi2c2)!=HAL_OK)
  // {
  //   Error_Handler();
  // }
  // watchdog_init();
    // MX_TIM2_Init();      // optional, for timing
    flash_init();        // Init external flash
    watchdog_init();

    HAL_I2C_EnableListen_IT(&hi2c2); // Start I2C listening

    // Initialize ArduCam
    // arducam_init();           // your function to init camera
    arducam_set_jpeg_mode();  // apply default brightness, contrast, etc.

    printf("Slave ready and listening for I2C commands...\n");


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
   while (1)
  {

  //   if (capture_requested)
  //   {
  //     arducam_set_jpeg_mode();
  //     HAL_Delay(2000);
  //     process_image_capture();
  //     capture_requested = 0;
  //   }

  //   if (HAL_GetTick() - last_i2c_check_time > 1000)  // every 1s
  //   {
  //     last_i2c_check_time = HAL_GetTick();

  //     if ((HAL_GetTick() - last_i2c_activity > 3000) &&
  //         (__HAL_I2C_GET_FLAG(&hi2c2, I2C_FLAG_BUSY)))
  //     {
  //       printf("I2C timeout detected! Resetting I2C2...\r\n");

  //       __HAL_RCC_I2C2_FORCE_RESET();
  //       HAL_Delay(2);
  //       __HAL_RCC_I2C2_RELEASE_RESET();
  //       MX_I2C2_Init();  
  //       HAL_I2C_EnableListen_IT(&hi2c2);

  //       capture_requested = 0;
  //       prestore_image_requested = 0;
  //       send_all_packets = 0;
  //       current_packet_index = 0;

  //       // Reset watchdog period after bus recovery
  //       watchdog_set_time_period(WD_DELAY_1_S);
  //     }
  //   }
  // }
  // Check for capture request
        if (capture_requested)
        {
            capture_requested = 0;   // clear flag
            printf("Capturing image...\n");
            process_image_capture(); // This handles capture + SSDV + flash storage
        }

        // Optional: monitor I2C bus timeout
        // check_i2c_timeout();

        // Feed watchdog regularly
        // watchdog_feed();
        watchdog_toggle();

 
     
  }
  //   /* USER CODE END WHILE */

  //   /* USER CODE BEGIN 3 */

 
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
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL2;
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
