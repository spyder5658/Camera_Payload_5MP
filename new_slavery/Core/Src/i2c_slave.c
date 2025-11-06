#include "main.h"
#include "i2c_slave.h"
#include "camera.h"
#include "test_card_ssdv.h"
#include "watchdog.h"

#define CAM_BRIGHTNESS_LEVEL_CMD  0x30
#define CAM_SHARPNESS_LEVEL_CMD   0x40
#define CAM_CONTRAST_LEVEL_CMD    0x50
#define CAM_EV_LEVEL_CMD          0x60
#define CAM_STAURATION_LEVEL_CMD  0x70
#define CAM_COLOR_FX_CMD          0x80
#define CAM_SIZE_CMD              0x81



volatile CAM_BRIGHTNESS_LEVEL lvl = 0;
volatile CAM_SHARPNESS_LEVEL shrp_lvl = 0;
volatile CAM_CONTRAST_LEVEL contra_lvl = 0;
volatile CAM_EV_LEVEL exposure_lvl = 0;
volatile CAM_STAURATION_LEVEL saturation_lvl = 0;
volatile CAM_COLOR_FX effect = 0;
volatile CAM_IMAGE_MODE cam_size = CAM_IMAGE_MODE_128X128;

volatile uint8_t capture_requested = 0;
volatile uint8_t prestore_image_requested = 0;

volatile uint8_t send_all_packets = 0;
volatile uint16_t current_packet_index = 0;

extern uint32_t ssdv_start_addr;
extern uint32_t ssdv_packets_in_image;

extern I2C_HandleTypeDef hi2c2;
#define RxSize 2
uint8_t RxData[RxSize];

uint32_t last_i2c_activity = 0; // ⬅️ Added for timeout tracking


void HAL_I2C_ListenCpltCallback(I2C_HandleTypeDef *hi2c)
{
    HAL_I2C_EnableListen_IT(hi2c);
}


void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
    HAL_I2C_EnableListen_IT(hi2c);
}

// void check_i2c_timeout()
// {
//     if (HAL_GetTick() - last_i2c_activity > 3000) // 3 seconds no I2C activity
//     {
//         if (__HAL_I2C_GET_FLAG(&hi2c2, I2C_FLAG_BUSY))
//         {
//             printf("I2C bus stuck, resetting...\r\n");
//             __HAL_RCC_I2C2_FORCE_RESET();
//             HAL_Delay(1);
//             __HAL_RCC_I2C2_RELEASE_RESET();
//             MX_I2C2_Init();  // Your re-init function
//             HAL_I2C_EnableListen_IT(&hi2c2);

//             // Optional: clear flags
//             capture_requested = 0;
//             send_all_packets = 0;
//             prestore_image_requested = 0;
//             current_packet_index = 0;
//         }
//         last_i2c_activity = HAL_GetTick(); // Prevent repeated reset
//     }
// }


void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c, uint8_t TransferDirection, uint16_t AddrMatchCode)
{
    last_i2c_activity = HAL_GetTick(); // ⬅️ Activity timestamp

    if (TransferDirection == I2C_DIRECTION_TRANSMIT)
    {
        HAL_I2C_Slave_Seq_Receive_IT(hi2c, RxData, RxSize, I2C_FIRST_AND_LAST_FRAME);
    }
    else if (TransferDirection == I2C_DIRECTION_RECEIVE)
    {
        if (prestore_image_requested)
        {
            if (current_packet_index < 8)
            {
                HAL_I2C_Slave_Seq_Transmit_IT(hi2c,
                    &test_card_ssdv_buffer[current_packet_index * SSDV_PKT_SIZE],
                    SSDV_PKT_SIZE,
                    I2C_FIRST_AND_LAST_FRAME);
                printf("Sent test SSDV packet %d\n", current_packet_index);
                current_packet_index++;
            }
            else
            {
                uint8_t dummy[1] = {0xFF};
                HAL_I2C_Slave_Seq_Transmit_IT(hi2c, dummy, 1, I2C_FIRST_AND_LAST_FRAME);
                prestore_image_requested = 0;
                current_packet_index = 0;
                watchdog_set_time_period(WD_DELAY_1_S);
                while (1); // 
            }
        }
        else if (send_all_packets && current_packet_index < ssdv_packets_in_image)
        {
            uint8_t ssdv_pkt[SSDV_PKT_SIZE] = {0};
            flash_read_buffer(ssdv_pkt,
                ssdv_start_addr + current_packet_index * SSDV_PKT_SIZE,
                SSDV_PKT_SIZE);

            HAL_I2C_Slave_Seq_Transmit_IT(hi2c, ssdv_pkt, SSDV_PKT_SIZE, I2C_FIRST_AND_LAST_FRAME);
            printf("Sent flash SSDV packet %d\n", current_packet_index);
            current_packet_index++;
        }
        else
        {
            uint8_t dummy[1] = {0xFF};
            HAL_I2C_Slave_Seq_Transmit_IT(hi2c, dummy, 1, I2C_FIRST_AND_LAST_FRAME);
            send_all_packets = 0;
            current_packet_index = 0;
            watchdog_set_time_period(WD_DELAY_1_S);
            while (1); // 
        }
    }
}

void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    last_i2c_activity = HAL_GetTick(); // ⬅️ Activity timestamp

    uint8_t cmd = RxData[0];

    if (cmd == 0x10) // Capture
    {
        printf("Capture command received\r\n");
        capture_requested = 1;
        send_all_packets = 0;
        prestore_image_requested = 0;
        current_packet_index = 0;
    }
    else if (cmd == 0x20) // Stream SSDV
    {
        printf("SSDV stream requested\r\n");
        send_all_packets = 1;
        current_packet_index = 0;
    }
    else if (cmd == 0x90) // Prestored
    {
        printf("Prestored command received\r\n");
        prestore_image_requested = 1;
        send_all_packets = 1;
        current_packet_index = 0;
    }
    else if (cmd == CAM_BRIGHTNESS_LEVEL_CMD)
    {
        lvl = RxData[1];
        printf("Brightness level: %i\n", lvl);
    }
    else if (cmd == CAM_SHARPNESS_LEVEL_CMD)
    {
        shrp_lvl = RxData[1];
        printf("Sharpness level: %i\n", shrp_lvl);
    }
    else if (cmd == CAM_CONTRAST_LEVEL_CMD)
    {
        contra_lvl = RxData[1];
        printf("Contrast level: %i\n", contra_lvl);
    }
    else if (cmd == CAM_EV_LEVEL_CMD)
    {
        exposure_lvl = RxData[1];
        printf("Exposure level: %i\n", exposure_lvl);
    }
    else if (cmd == CAM_STAURATION_LEVEL_CMD)
    {
        saturation_lvl = RxData[1];
        printf("Saturation level: %i\n", saturation_lvl);
    }
    else if (cmd == CAM_COLOR_FX_CMD)
    {
        effect = RxData[1];
        printf("Effect: %i\n", effect);
    }
    else if (cmd == CAM_SIZE_CMD)
    {
        cam_size =  RxData[1];
        printf("Cam Size: %i\n", cam_size);
    }
}
