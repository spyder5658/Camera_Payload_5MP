#include "main.h"
#include "i2c_slave.h"
#include "camera.h"


#define CAM_BRIGHTNESS_LEVEL_CMD  0x30
#define CAM_SHARPNESS_LEVEL_CMD   0x40
#define CAM_CONTRAST_LEVEL_CMD    0x50
#define CAM_EV_LEVEL_CMD          0x60
#define CAM_STAURATION_LEVEL_CMD  0x70
#define CAM_COLOR_FX_CMD          0x80



volatile CAM_BRIGHTNESS_LEVEL lvl = 0;
volatile CAM_SHARPNESS_LEVEL shrp_lvl = 0;
volatile CAM_CONTRAST_LEVEL contra_lvl = 0;
volatile CAM_EV_LEVEL exposure_lvl = 0;
volatile CAM_STAURATION_LEVEL saturation_lvl = 0;
volatile CAM_COLOR_FX effect = 0;

volatile uint8_t capture_requested = 0;

volatile uint8_t send_all_packets = 0;
volatile uint16_t current_packet_index = 0;


extern uint32_t ssdv_start_addr;
extern uint32_t ssdv_packets_in_image;

extern I2C_HandleTypeDef hi2c2;
#define RxSize 2
uint8_t RxData[RxSize];

int rxcount = 0 ;
// extern ssdv_packets_in_image, ssdv_start_addr;


void HAL_I2C_ListenCpltCallback(I2C_HandleTypeDef *hi2c)
{
    HAL_I2C_EnableListen_IT(hi2c);
}



void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c, uint8_t TransferDirection, uint16_t AddrMatchCode)
{
    if (TransferDirection == I2C_DIRECTION_TRANSMIT) {
        HAL_I2C_Slave_Seq_Receive_IT(hi2c, RxData, RxSize, I2C_FIRST_AND_LAST_FRAME);
    } 
    else if (TransferDirection == I2C_DIRECTION_RECEIVE) {
        if (send_all_packets && current_packet_index < ssdv_packets_in_image) {
            uint8_t ssdv_pkt[SSDV_PKT_SIZE] = {0};
            flash_read_buffer(ssdv_pkt, ssdv_start_addr + current_packet_index * SSDV_PKT_SIZE, SSDV_PKT_SIZE);

            HAL_I2C_Slave_Seq_Transmit_IT(hi2c, ssdv_pkt, SSDV_PKT_SIZE, I2C_FIRST_AND_LAST_FRAME);
            printf("Sent SSDV packet %d\n", current_packet_index);
            current_packet_index++;
        } else {
            // All packets sent or transfer not started
            uint8_t dummy[1] = {0xFF};
            HAL_I2C_Slave_Seq_Transmit_IT(hi2c, dummy, 1, I2C_FIRST_AND_LAST_FRAME);
            send_all_packets = 0;  // Reset stream mode
        }
    }
}


void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    rxcount++;
    uint8_t cmd = RxData[0];

    if (cmd == 0x10) 
    {
        printf("Capture command received\r\n");
        capture_requested = 1;
    }
    else if (cmd == 0x20) 
    {
        printf("SSDV stream requested\r\n");
        send_all_packets = 1;
        current_packet_index = 0;
    }
    else if(cmd == CAM_BRIGHTNESS_LEVEL_CMD )
    {
        lvl = RxData[1];
        printf("brightness level: %i \n",lvl);

    }
    else if(cmd == CAM_SHARPNESS_LEVEL_CMD )
    {
        shrp_lvl = RxData[1];
        printf("sharpness level: %i \n",lvl);

    }
    else if(cmd == CAM_CONTRAST_LEVEL_CMD )
    {
        contra_lvl = RxData[1];
        printf("contrast level: %i \n",lvl);

    }
    else if(cmd == CAM_EV_LEVEL_CMD )
    {
        exposure_lvl = RxData[1];
        printf("exposure level: %i \n",lvl);

    }
    else if(cmd == CAM_STAURATION_LEVEL_CMD )
    {
        saturation_lvl = RxData[1];
        printf("saturation level: %i \n",lvl);

    }
     else if(cmd == CAM_COLOR_FX_CMD )
    {
        effect = RxData[1];
        printf("effect : %i \n",lvl);

    }
    
}



void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
    HAL_I2C_EnableListen_IT(hi2c);
}



