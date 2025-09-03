
#include "camera.h"

#include "mini_morse.h"
#include "radio.h"

uint32_t last_i2c_activity = 0; // Added for timeout tracking



void request_image_capture() 
{
     last_i2c_activity = HAL_GetTick();
    uint8_t cmd[2] = {0x10,0};
    if (HAL_I2C_Master_Transmit(&hi2c2, SSDV_SLAVE_ADDR, cmd, 2, HAL_MAX_DELAY) == HAL_OK) {
        printf("Image capture command sent.\r\n");
    } else {
        printf("Failed to send image capture command.\r\n");
    }
}

void request_prestored() 
{
     last_i2c_activity = HAL_GetTick();
    uint8_t cmd[2] = {0x90,0};
    if (HAL_I2C_Master_Transmit(&hi2c2, SSDV_SLAVE_ADDR, cmd, 2, HAL_MAX_DELAY) == HAL_OK) {
        printf("prestored command sent.\r\n");
    } else {
        printf("Failed to send prestored command.\r\n");
    }
}

void request_ssdv_stream() 
{
    uint8_t cmd[2] = {0x20,0};
    if (HAL_I2C_Master_Transmit(&hi2c2, SSDV_SLAVE_ADDR, cmd, 2, HAL_MAX_DELAY) == HAL_OK) {
        printf("SSDV stream command sent.\r\n");
    } else {
        printf("Failed to send SSDV stream command.\r\n");
    }
}

void read_ssdv_stream() 
{
     last_i2c_activity = HAL_GetTick();
    uint8_t ssdv_pkt[SSDV_PKT_SIZE];
    uint16_t packet_index = 0;
    radio_init_gfsk(GFSK_500BPS_1KHZ);
    

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
        // printf("transmitting through gsk\r\n");
        // printf("\n-----------------------------\n");
        radio_tx_gfsk(ssdv_pkt,sizeof(ssdv_pkt)/sizeof(ssdv_pkt[0]));

        // HAL_Delay(2000);

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



void request_prestored_image(void)
{
     last_i2c_activity = HAL_GetTick();
    HAL_StatusTypeDef res;

    // Step 1: Send 0x90 command to request prestored image
    uint8_t cmd[2] = {0x90, 0x00}; // second byte is ignored
    res = HAL_I2C_Master_Transmit(&hi2c2, SSDV_SLAVE_ADDR, cmd, 2, HAL_MAX_DELAY);
    if (res != HAL_OK) {
        printf("Failed to send prestored command\n");
        return;
    }

    read_ssdv_stream();
}


void camera_on()
{
    HAL_GPIO_WritePin(Payload_switch_GPIO_Port,Payload_switch_Pin,SET);
}

void camera_off()
{
    HAL_GPIO_WritePin(Payload_switch_GPIO_Port,Payload_switch_Pin,RESET);


}







void check_i2c_timeout()
{
    if (HAL_GetTick() - last_i2c_activity > 3000) // 3 seconds no I2C activity
    {
        if (__HAL_I2C_GET_FLAG(&hi2c2, I2C_FLAG_BUSY))
        {
            printf("I2C bus stuck, resetting...\r\n");
            __HAL_RCC_I2C2_FORCE_RESET();
            HAL_Delay(1);
            __HAL_RCC_I2C2_RELEASE_RESET();
            MX_I2C2_Init();  // Your re-init function
            HAL_I2C_EnableListen_IT(&hi2c2);

        }
        last_i2c_activity = HAL_GetTick(); // Prevent repeated reset
    }
}


 uint32_t last_i2c_check_time = 0;

void i2c_check()
{
    if (HAL_GetTick() - last_i2c_check_time > 1000)  // every 1s
    {
    last_i2c_check_time = HAL_GetTick();

    // Timeout: if no I2C activity in 3s, and BUSY flag still set
    if ((HAL_GetTick() - last_i2c_activity > 3000) &&                    // uint32_t last_i2c_check_time = 0;   use this at the beginning if not declared;
        (__HAL_I2C_GET_FLAG(&hi2c2, I2C_FLAG_BUSY)))
    {
        printf("I2C timeout detected! Resetting I2C2...\r\n");

        // Reset I2C2 peripheral safely
        __HAL_RCC_I2C2_FORCE_RESET();
        HAL_Delay(2);
        __HAL_RCC_I2C2_RELEASE_RESET();
        MX_I2C2_Init();  // Reinitialize I2C2
    

    }
    }
}

void camera_request_payload()
{
    request_image_capture();
    i2c_check();
    // Wait
    HAL_UART_Transmit(&huart1, (uint8_t*)"/* ", 3, HAL_MAX_DELAY);
    for (uint8_t i = 0; i < 8; i++)
    {
        HAL_UART_Transmit(&huart1, (uint8_t*)"!", 1, HAL_MAX_DELAY);
        HAL_Delay(1000);
    }
    HAL_UART_Transmit(&huart1, (uint8_t*)" */\n", 4, HAL_MAX_DELAY);

    // Step 2: Request SSDV Stream
    request_ssdv_stream();
    HAL_Delay(200);

    // Step 3: Read All SSDV Packets
    read_ssdv_stream();
    // camera_off();

}

















