#ifndef _I2C_SLAVE_H_
#define _I2C_SLAVE_H_

#include <stdio.h>
#include <inttypes.h>

extern volatile uint8_t capture_requested;
extern volatile uint8_t send_all_packets ;
extern volatile uint16_t current_packet_index;


#endif