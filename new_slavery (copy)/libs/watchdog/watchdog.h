#ifndef WATCHDOG_H
#define WATCHDOG_H

#include "gpio.h"
#include "usart.h"

typedef enum
{
  WD_DELAY_1_MS,  // not observable
  WD_DELAY_10_MS, // not observable
  WD_DELAY_54_MS, // 30 ms but observed 54 ms
  WD_DELAY_DISABLE,
  WD_DELAY_1_S,   // 100 ms but observed 1.734 s
  WD_DELAY_3_S,   // 1 s but observed 3.275 s
  WD_DELAY_32_S,  // 10 s but observed 32 s
  WD_DELAY_3_MIN, // 1 min but observed 3.2 min
} watchdog_delay;

void watchdog_init(void);
void watchdog_toggle(void);
void watchdog_disable(void);
void watchdog_set_time_period(const watchdog_delay wdd);

#endif // WATCHDOG_H
