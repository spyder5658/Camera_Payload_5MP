#include "watchdog.h"

// Tracks watchdog state
static uint8_t watchdog_toggle_flag = 0;

void watchdog_init(void)
{
  watchdog_disable();
}

void watchdog_disable(void)
{
  HAL_GPIO_WritePin(SET2_GPIO_Port, SET2_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(SET1_GPIO_Port, SET1_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(SET0_GPIO_Port, SET0_Pin, GPIO_PIN_SET);
}

// Reset the watchdog timer
void watchdog_toggle(void)
{
  HAL_GPIO_WritePin(WDToggle_GPIO_Port, WDToggle_Pin, (GPIO_PinState)watchdog_toggle_flag);
  watchdog_toggle_flag = !watchdog_toggle_flag;
}

// Function to set the watchdog timer period
void watchdog_set_time_period(const watchdog_delay wdd)
{
  switch(wdd)
  {
    case WD_DELAY_1_MS:
    {
      HAL_GPIO_WritePin(SET2_GPIO_Port, SET2_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(SET1_GPIO_Port, SET1_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(SET0_GPIO_Port, SET0_Pin, GPIO_PIN_RESET);
      break;
    }

    case WD_DELAY_10_MS:
    {
      HAL_GPIO_WritePin(SET2_GPIO_Port, SET2_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(SET1_GPIO_Port, SET1_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(SET0_GPIO_Port, SET0_Pin, GPIO_PIN_SET);
      break;
    }

    case WD_DELAY_54_MS:
    {
      HAL_GPIO_WritePin(SET2_GPIO_Port, SET2_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(SET1_GPIO_Port, SET1_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(SET0_GPIO_Port, SET0_Pin, GPIO_PIN_RESET);
      break;
    }

    case WD_DELAY_DISABLE:
    {
      watchdog_disable();
      break;
    }

    case WD_DELAY_1_S:
    {
      HAL_GPIO_WritePin(SET2_GPIO_Port, SET2_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(SET1_GPIO_Port, SET1_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(SET0_GPIO_Port, SET0_Pin, GPIO_PIN_RESET);
      break;
    }

    case WD_DELAY_3_S:
    {
      HAL_GPIO_WritePin(SET2_GPIO_Port, SET2_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(SET1_GPIO_Port, SET1_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(SET0_GPIO_Port, SET0_Pin, GPIO_PIN_SET);
      break;
    }

    case WD_DELAY_32_S:
    {
      HAL_GPIO_WritePin(SET2_GPIO_Port, SET2_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(SET1_GPIO_Port, SET1_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(SET0_GPIO_Port, SET0_Pin, GPIO_PIN_RESET);
      break;
    }

    case WD_DELAY_3_MIN:
    {
      HAL_GPIO_WritePin(SET2_GPIO_Port, SET2_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(SET1_GPIO_Port, SET1_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(SET0_GPIO_Port, SET0_Pin, GPIO_PIN_SET);
      break;
    }

    default:
    {
      watchdog_disable();
      break;
    }
  }
}
