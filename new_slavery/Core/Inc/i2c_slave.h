#ifndef _I2C_SLAVE_H_
#define _I2C_SLAVE_H_

#include <stdio.h>
#include <inttypes.h>
#include "camera.h"

extern volatile uint8_t capture_requested;
extern volatile uint8_t send_all_packets ;
extern volatile uint16_t current_packet_index;
extern volatile CAM_BRIGHTNESS_LEVEL lvl;
extern volatile CAM_SHARPNESS_LEVEL shrp_lvl;
extern volatile CAM_CONTRAST_LEVEL contra_lvl;
extern volatile CAM_EV_LEVEL exposure_lvl;
extern volatile CAM_STAURATION_LEVEL saturation_lvl;
extern volatile CAM_COLOR_FX effect;






#endif