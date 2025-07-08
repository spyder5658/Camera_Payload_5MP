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
int _write(int file, char *data, int len) {
    HAL_UART_Transmit(&huart1, (uint8_t*)data, len, HAL_MAX_DELAY);
    return len;
}
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

#define SSDV_SLAVE_ADDR (0x12<<1)


// uint8_t SSDV_Buffer[SSDV_PKT_SIZE];      // Buffer to hold each received packet


#define SSDV_PKT_SIZE       224
#define SSDV_MAX_PACKETS    255

void request_image_capture() {
    uint8_t cmd[2] = {0x10,0};
    if (HAL_I2C_Master_Transmit(&hi2c2, SSDV_SLAVE_ADDR, cmd, 2, HAL_MAX_DELAY) == HAL_OK) {
        printf("Image capture command sent.\r\n");
    } else {
        printf("Failed to send image capture command.\r\n");
    }
}

void request_ssdv_stream() {
    uint8_t cmd[2] = {0x20,0};
    if (HAL_I2C_Master_Transmit(&hi2c2, SSDV_SLAVE_ADDR, cmd, 2, HAL_MAX_DELAY) == HAL_OK) {
        printf("SSDV stream command sent.\r\n");
    } else {
        printf("Failed to send SSDV stream command.\r\n");
    }
}

void read_ssdv_stream() {
    uint8_t ssdv_pkt[SSDV_PKT_SIZE];
    uint16_t packet_index = 0;

    while (1) {
        if (HAL_I2C_Master_Receive(&hi2c2, SSDV_SLAVE_ADDR, ssdv_pkt, SSDV_PKT_SIZE, HAL_MAX_DELAY) != HAL_OK) {
            printf("Error receiving packet %d\r\n", packet_index);
            break;
        }

        // Stop when dummy packet 0xFF is received
        if (ssdv_pkt[0] == 0xFF) {
            printf("End of SSDV stream reached.\r\n");
            break;
        }

        // Print packet content (optional)
        printf("Packet %d received:\n", packet_index);
        for (int i = 0; i < SSDV_PKT_SIZE; i++) {
            printf("0x%02X, ", ssdv_pkt[i]);
            if ((i + 1) % 16 == 0) printf("\n");
        }
        printf("\n-----------------------------\n");

        packet_index++;
        if (packet_index >= SSDV_MAX_PACKETS) {
            printf("Max SSDV packet limit reached.\r\n");
            break;
        }
    }
}

#define CAM_BRIGHTNESS_LEVEL_CMD  0x30
#define CAM_SHARPNESS_LEVEL_CMD   0x40
#define CAM_CONTRAST_LEVEL_CMD    0x50
#define CAM_EV_LEVEL_CMD          0x60
#define CAM_STAURATION_LEVEL_CMD  0x70
#define CAM_COLOR_FX_CMD          0x80


typedef enum {
    CAM_BRIGHTNESS_LEVEL_MINUS_4 = 8, /**<Level -4 */
    CAM_BRIGHTNESS_LEVEL_MINUS_3 = 6, /**<Level -3 */
    CAM_BRIGHTNESS_LEVEL_MINUS_2 = 4, /**<Level -2 */
    CAM_BRIGHTNESS_LEVEL_MINUS_1 = 2, /**<Level -1 */
    CAM_BRIGHTNESS_LEVEL_DEFAULT = 0, /**<Level Default*/
    CAM_BRIGHTNESS_LEVEL_1       = 1, /**<Level +1 */
    CAM_BRIGHTNESS_LEVEL_2       = 3, /**<Level +2 */
    CAM_BRIGHTNESS_LEVEL_3       = 5, /**<Level +3 */
    CAM_BRIGHTNESS_LEVEL_4       = 7, /**<Level +4 */
} CAM_BRIGHTNESS_LEVEL;

typedef enum {
    CAM_SHARPNESS_LEVEL_AUTO = 0, /**<Sharpness Auto */
    CAM_SHARPNESS_LEVEL_1,        /**<Sharpness Level 1 */
    CAM_SHARPNESS_LEVEL_2,        /**<Sharpness Level 2 */
    CAM_SHARPNESS_LEVEL_3,        /**<Sharpness Level 3 */
    CAM_SHARPNESS_LEVEL_4,        /**<Sharpness Level 4 */
    CAM_SHARPNESS_LEVEL_5,        /**<Sharpness Level 5 */
    CAM_SHARPNESS_LEVEL_6,        /**<Sharpness Level 6 */
    CAM_SHARPNESS_LEVEL_7,        /**<Sharpness Level 7 */
    CAM_SHARPNESS_LEVEL_8,        /**<Sharpness Level 8 */
} CAM_SHARPNESS_LEVEL;


typedef enum {
    CAM_CONTRAST_LEVEL_MINUS_3 = 6, /**<Level -3 */
    CAM_CONTRAST_LEVEL_MINUS_2 = 4, /**<Level -2 */
    CAM_CONTRAST_LEVEL_MINUS_1 = 2, /**<Level -1 */
    CAM_CONTRAST_LEVEL_DEFAULT = 0, /**<Level Default*/
    CAM_CONTRAST_LEVEL_1       = 1, /**<Level +1 */
    CAM_CONTRAST_LEVEL_2       = 3, /**<Level +2 */
    CAM_CONTRAST_LEVEL_3       = 5, /**<Level +3 */
} CAM_CONTRAST_LEVEL;


typedef enum {
    CAM_EV_LEVEL_MINUS_3 = 6, /**<Level -3 */
    CAM_EV_LEVEL_MINUS_2 = 4, /**<Level -2 */
    CAM_EV_LEVEL_MINUS_1 = 2, /**<Level -1 */
    CAM_EV_LEVEL_DEFAULT = 0, /**<Level Default*/
    CAM_EV_LEVEL_1       = 1, /**<Level +1 */
    CAM_EV_LEVEL_2       = 3, /**<Level +2 */
    CAM_EV_LEVEL_3       = 5, /**<Level +3 */
} CAM_EV_LEVEL;


typedef enum {
    CAM_STAURATION_LEVEL_MINUS_3 = 6, /**<Level -3 */
    CAM_STAURATION_LEVEL_MINUS_2 = 4, /**<Level -2 */
    CAM_STAURATION_LEVEL_MINUS_1 = 2, /**<Level -1 */
    CAM_STAURATION_LEVEL_DEFAULT = 0, /**<Level Default*/
    CAM_STAURATION_LEVEL_1       = 1, /**<Level +1 */
    CAM_STAURATION_LEVEL_2       = 3, /**<Level +2 */
    CAM_STAURATION_LEVEL_3       = 5, /**<Level +3 */
} CAM_STAURATION_LEVEL;


typedef enum {
    CAM_COLOR_FX_NONE = 0,      /**< no effect   */
    CAM_COLOR_FX_BLUEISH,       /**< cool light   */
    CAM_COLOR_FX_REDISH,        /**< warm   */
    CAM_COLOR_FX_BW,            /**< Black/white   */
    CAM_COLOR_FX_SEPIA,         /**<Sepia   */
    CAM_COLOR_FX_NEGATIVE,      /**<positive/negative inversion  */
    CAM_COLOR_FX_GRASS_GREEN,   /**<Grass green */
    CAM_COLOR_FX_OVER_EXPOSURE, /**<Over exposure*/ //redish
    CAM_COLOR_FX_SOLARIZE,      /**< Solarize   */
} CAM_COLOR_FX;



void set_brightness_(CAM_BRIGHTNESS_LEVEL level )
 {
    uint8_t cmd[2];
    cmd[0] = CAM_BRIGHTNESS_LEVEL_CMD;
    cmd [1] = level ;
    if (HAL_I2C_Master_Transmit(&hi2c2, SSDV_SLAVE_ADDR, cmd, 2, HAL_MAX_DELAY) == HAL_OK) {
        printf("brightness set.\r\n");
    } else {
        printf("Failed to set brigtness.\r\n");
    }
}

void set_sharpness_(CAM_SHARPNESS_LEVEL level )
 {
    uint8_t cmd[2];
    cmd[0] = CAM_SHARPNESS_LEVEL_CMD;
    cmd [1] = level ;
    if (HAL_I2C_Master_Transmit(&hi2c2, SSDV_SLAVE_ADDR, cmd, 2, HAL_MAX_DELAY) == HAL_OK) {
        printf("sharpness set.\r\n");
    } else {
        printf("Failed to set sharpness.\r\n");

    }
}

void set_contrast_(CAM_CONTRAST_LEVEL level )
 {
    uint8_t cmd[2];
    cmd[0] = CAM_CONTRAST_LEVEL_CMD;
    cmd [1] = level ;
    if (HAL_I2C_Master_Transmit(&hi2c2, SSDV_SLAVE_ADDR, cmd, 2, HAL_MAX_DELAY) == HAL_OK) {
        printf("contrast set.\r\n");
    } else {
        printf("Failed to set contrast.\r\n");

    }
}

void set_exposure_(CAM_EV_LEVEL level )
 {
    uint8_t cmd[2];
    cmd[0] = CAM_EV_LEVEL_CMD;
    cmd [1] = level ;
    if (HAL_I2C_Master_Transmit(&hi2c2, SSDV_SLAVE_ADDR, cmd, 2, HAL_MAX_DELAY) == HAL_OK) {
        printf("expsoure set.\r\n");
    } else {
        printf("Failed to set expsoure.\r\n");

    }
}

void set_saturation_(CAM_STAURATION_LEVEL level )
 {
    uint8_t cmd[2];
    cmd[0] = CAM_STAURATION_LEVEL_CMD;
    cmd [1] = level ;
    if (HAL_I2C_Master_Transmit(&hi2c2, SSDV_SLAVE_ADDR, cmd, 2, HAL_MAX_DELAY) == HAL_OK) {
        printf("saturation set.\r\n");
    } else {
        printf("Failed to set saturation.\r\n");

    }
}

void set_effect_(CAM_COLOR_FX effect )
 {
    uint8_t cmd[2];
    cmd[0] = CAM_COLOR_FX_CMD;
    cmd [1] = effect ;
    if (HAL_I2C_Master_Transmit(&hi2c2, SSDV_SLAVE_ADDR, cmd, 2, HAL_MAX_DELAY) == HAL_OK) {
        printf("effect set.\r\n");
    } else {
        printf("Failed to set effect.\r\n");

    }
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
  printf("Master starting...\r\n");
  set_brightness_(CAM_BRIGHTNESS_LEVEL_4);
  set_effect_(CAM_COLOR_FX_SEPIA );
  // set_contrast_(CAM_CONTRAST_LEVEL_MINUS_3 );

  // Step 1: Request Image Capture
  request_image_capture();
  HAL_Delay(8000);  // Allow time for image capture & SSDV encoding

  // Step 2: Request SSDV Stream
  request_ssdv_stream();
  HAL_Delay(200);

  // Step 3: Read All SSDV Packets
  read_ssdv_stream();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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
