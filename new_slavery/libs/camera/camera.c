#include "camera.h"
#include "i2c_slave.h"


uint8_t image_buffer[MAX_IMAGE_SIZE]={0};

#define JPEG_SOI 0xFFD8  // Start Of Image
#define JPEG_EOI 0xFFD9  // End Of Image
uint32_t jpeg_size = 0;


uint32_t ssdv_packets_in_image = 0;
uint32_t ssdv_start_addr = 0; 

/* USER CODE BEGIN PD */
extern SPI_HandleTypeDef hspi1;
char ssdv_type = SSDV_TYPE_NORMAL;
char ssdv_call_sign[7] = "CALSIGN";
uint8_t ssdv_image_id = 0;
int8_t ssdv_quality = 4;


void arducam_write_register(uint8_t reg, uint8_t value) {
    uint8_t tx_buf[2] = {reg | 0x80, value};
    CS_LOW(); // Assert CS
    HAL_SPI_Transmit(&hspi1, tx_buf, 2, HAL_MAX_DELAY);
    CS_HIGH();
}

uint8_t arducam_read_register(uint8_t reg) {
    uint8_t tx_buf[2] = {reg & 0x7F, 0x00}; // Second byte is dummy
    uint8_t rx_buf[2];
    CS_LOW(); // Assert CS
    HAL_SPI_TransmitReceive(&hspi1, tx_buf, rx_buf, 2, HAL_MAX_DELAY);
    CS_HIGH(); // Assert CS

    return rx_buf[1]; // Received data
}



// // Burst read from FIFO
// uint32_t arducam_burst_read(uint8_t *buf, uint32_t max_len) {
//     uint8_t burst_cmd = 0x3C; // Burst read register
//     CS_LOW();
//     HAL_SPI_Transmit(&hspi1, &burst_cmd, 1, HAL_MAX_DELAY);
//     uint8_t dummy = 0x00;
//     HAL_SPI_Transmit(&hspi1, &dummy, 1, HAL_MAX_DELAY); // First dummy byte

//     HAL_SPI_Receive(&hspi1, buf, max_len, HAL_MAX_DELAY);
//     CS_HIGH();
//     return max_len;
// }


// Safer burst read from FIFO
uint32_t arducam_burst_read(uint8_t *buf, uint32_t max_len) {
    uint8_t burst_cmd = 0x3C;
    CS_LOW();
    HAL_SPI_Transmit(&hspi1, &burst_cmd, 1, HAL_MAX_DELAY);

    uint8_t dummy = 0x00;
    HAL_SPI_Transmit(&hspi1, &dummy, 1, HAL_MAX_DELAY);

    for (uint32_t i = 0; i < max_len; i++) {
        uint8_t tx = 0x00;
        uint8_t rx = 0x00;
        if (HAL_SPI_TransmitReceive(&hspi1, &tx, &rx, 1, HAL_MAX_DELAY) != HAL_OK) {
            CS_HIGH();
            return i;
        }
        buf[i] = rx;
    }
    CS_HIGH();
    return max_len;
}

void camersSetBrightness(CAM_BRIGHTNESS_LEVEL level )
{
  arducam_write_register(0x22,level);
}

void cameraSetSharpness(CAM_SHARPNESS_LEVEL level)
{
  arducam_write_register(0x28,level);

}

void cameraSetContrast (CAM_CONTRAST_LEVEL level)
{
  arducam_write_register(0x23,level);
}

void cameraSetEV (CAM_EV_LEVEL level)
{
  arducam_write_register(0x25,level);

}

void cameraSetSaturation (CAM_STAURATION_LEVEL level)
{
  arducam_write_register(0x24,level);

}

void cameraSetColorEffect (CAM_COLOR_FX effect)
{
  arducam_write_register(0x27,effect);

}

void arducam_set_jpeg_mode() {
    // Write JPEG basic configuration to register 0x20
    arducam_write_register(0x20, 0x01);  // JPEG mode
    HAL_Delay(10);  // Small delay for settings to apply

     // 2. Set resolution to 128x128
    arducam_write_register(0x21, cam_size);  // Resolution = 128x128
    HAL_Delay(10);

    camersSetBrightness(lvl);
    printf("brighness level:%i \n" ,lvl);
    HAL_Delay(10);

    cameraSetSaturation(saturation_lvl);
    printf("saturation level:%i \n" ,saturation_lvl);

    HAL_Delay(10);

    cameraSetEV(exposure_lvl);
    printf("exposure level:%i \n" ,exposure_lvl);

    HAL_Delay(10);

    cameraSetContrast(contra_lvl);
    printf("contrast level:%i \n" ,contra_lvl);

    HAL_Delay(10);

    cameraSetSharpness(shrp_lvl);
    printf("sharpness level:%i \n" ,shrp_lvl);

    HAL_Delay(10);

    cameraSetColorEffect(effect);
    printf("effect:%i \n" ,effect);
    HAL_Delay(10);

}


// Capture image
void arducam_capture_image() {
    // 1. Clear FIFO write done flag
    arducam_write_register(0x04, 0x01);
    HAL_Delay(10);

    // 2. Start image capture
    arducam_write_register(0x04, 0x02);
    HAL_Delay(200);  // Adjust delay as needed

    // Optional: Wait until FIFO done (polling or timeout)
    // If camera sets flag in 0x01 (frames written), you could poll here
}


uint32_t find_jpeg( uint32_t bytes_read,  uint8_t image_buffer[MAX_IMAGE_SIZE])    
{
    // Find JPEG start (0xFFD8) and end (0xFFD9)
    uint32_t jpeg_length=0;
    int start_index = -1, end_index = -1;
    for (uint32_t i = 0; i < bytes_read - 1; i++) {
        if (image_buffer[i] == 0xFF && image_buffer[i + 1] == 0xD8 && start_index == -1) {
            start_index = i;
        }
        if (image_buffer[i] == 0xFF && image_buffer[i + 1] == 0xD9) {
            end_index = i + 1;
            break;
        }
    }

    if (start_index != -1 && end_index != -1 && end_index > start_index) 
    {
        printf("JPEG found: start=%d, end=%d\n", start_index, end_index);
        // for (int i = start_index; i <= end_index; i++) {
        //     printf("%02X ", image_buffer[i]);
        // }
        // printf("\nJPEG extraction complete (%d bytes).\n", end_index - start_index + 1);
        jpeg_length=end_index - start_index + 1;
    } 
    // uint32_t jpeg_length=end_index - start_index + 1;
    return jpeg_length;


    // else {
        // printf("JPEG not found in the image buffer.\n");
        // return false;

    // }
}


// //5th iteration
void process_image_capture()
{
    current_packet_index = 0;

    // printf("setttinggggg.....camera\r\n");
    arducam_set_jpeg_mode();
    HAL_Delay(10);   
    printf("Starting image capture...\n");
    arducam_capture_image();

    printf("Reading image from FIFO...\n");
    uint32_t bytes_read = arducam_burst_read(image_buffer, MAX_IMAGE_SIZE);
    uint32_t jpeg_length = 0;
    int start_index = -1, end_index = -1;

    // Find JPEG SOI and EOI markers
    for (uint32_t i = 0; i < bytes_read - 1; i++) {
        if (image_buffer[i] == 0xFF && image_buffer[i + 1] == 0xD8 && start_index == -1) {
            start_index = i;
        }
        if (image_buffer[i] == 0xFF && image_buffer[i + 1] == 0xD9) {
            end_index = i + 1;
            break;
        }
    }

    if (start_index == -1 || end_index == -1 || end_index <= start_index) {
        printf("JPEG not found in buffer\n");
        return;
    }

    jpeg_length = end_index - start_index + 1;
    printf("JPEG found: start=%d, end=%d, length=%lu\n", start_index, end_index, jpeg_length);

    /*------ SSDV ENCODING --------*/
    ssdv_t ssdv;
    uint8_t ssdv_pkt[SSDV_PKT_SIZE];

    ssdv_enc_init(&ssdv, ssdv_type, ssdv_call_sign, ssdv_image_id++, ssdv_quality);
    ssdv_enc_set_buffer(&ssdv, ssdv_pkt);

    // size_t ssdv_packets_in_image = 0;

    // Flash initialization
    flash_init();

    uint32_t flash_id = flash_read_id();
    if (flash_id != FLASH_ID_512) {
        printf("Flash not found or not matching (ID: 0x%lX)\r\n", flash_id);
        while (1);
    } else {
        printf("Flash found (ID: 0x%lX)\r\n", flash_id);
    }

    // Address for current image
    uint32_t image_index = 0;  // Replace with a persistent value for multi-image storage
    uint32_t ssdv_start_addr = image_index * (SSDV_PKT_SIZE * MAX_SSDV_PACKETS);

    // Erase necessary sectors
    for (uint32_t addr = ssdv_start_addr; addr < (ssdv_start_addr + (SSDV_PKT_SIZE * MAX_SSDV_PACKETS)); addr += FLASH_SECTOR_SIZE) {
        flash_erase_sector(addr);
    }

    // SSDV encoding and flash writing
    uint32_t bi = 0;
    int c;
    while (1) {
        while ((c = ssdv_enc_get_packet(&ssdv)) == SSDV_FEED_ME) {
            uint32_t chunk_size = ((jpeg_length - bi) >= 128) ? 128 : (jpeg_length - bi);
            if (chunk_size == 0) break;
            ssdv_enc_feed(&ssdv, &image_buffer[start_index + bi], chunk_size);
            bi += chunk_size;
        }

        if (c == SSDV_EOI) {
            printf("EOI Packet.\n");
            break;
        } else if (c == SSDV_OK) {
            printf("Writing SSDV packet to flash...\n");
            flash_write_buffer(ssdv_pkt, ssdv_start_addr + ssdv_packets_in_image * SSDV_PKT_SIZE, SSDV_PKT_SIZE);
            printf("| SSDV packet %02lu stored at address 0x%06lX |\n", ssdv_packets_in_image + 1,
                   (unsigned long)(ssdv_start_addr + ssdv_packets_in_image * SSDV_PKT_SIZE));
            ssdv_packets_in_image++;
        } else {
            printf("[SSDV] Error or unexpected result.\n");
            break;
        }
    }

    /*------ READ BACK SSDV PACKETS --------*/
    printf("\n[READING BACK %lu SSDV PACKETS FROM FLASH]\n", ssdv_packets_in_image);
    for (uint32_t i = 0; i < ssdv_packets_in_image; i++) {
        uint8_t buffer[SSDV_PKT_SIZE] = {0};
        flash_read_buffer(buffer, ssdv_start_addr + i * SSDV_PKT_SIZE, SSDV_PKT_SIZE);
        HAL_Delay(10);

        printf("Packet %02lu: ", i + 1);
        for (uint16_t j = 0; j < SSDV_PKT_SIZE; j++) {
            printf("0x%02X, ", buffer[j]);
            if ((j + 1) % 16 == 0) printf("\n");
        }
        printf("\n-----------------------------\n");
    }

    printf("[FLASH READ COMPLETE]\r\n");

    current_packet_index = 0;
    send_all_packets = 0;
    prestore_image_requested = 0;
    // previous_send_flash_packets= 0;
}


// void process_image_capture()
// {
//     arducam_set_jpeg_mode();
//     printf("Starting image capture...\n");
//     arducam_capture_image();

//     printf("Reading image from FIFO...\n");
//     uint32_t bytes_read = arducam_burst_read(image_buffer, MAX_IMAGE_SIZE);
//     uint32_t jpeg_length = 0;
//     int start_index = -1, end_index = -1;

//     for (uint32_t i = 0; i < bytes_read - 1; i++) {
//         if (image_buffer[i] == 0xFF && image_buffer[i + 1] == 0xD8 && start_index == -1) start_index = i;
//         if (image_buffer[i] == 0xFF && image_buffer[i + 1] == 0xD9) {
//             end_index = i + 1;
//             break;
//         }
//     }

//     if (start_index == -1 || end_index == -1 || end_index <= start_index) {
//         printf("JPEG not found in buffer\n");
//         return;
//     }

//     jpeg_length = end_index - start_index + 1;
//     printf("JPEG found: start=%d, end=%d, length=%lu\n", start_index, end_index, jpeg_length);

//     ssdv_t ssdv;
//     uint8_t ssdv_pkt[SSDV_PKT_SIZE];
//     ssdv_enc_init(&ssdv, ssdv_type, ssdv_call_sign, ssdv_image_id++, ssdv_quality);
//     ssdv_enc_set_buffer(&ssdv, ssdv_pkt);

//     size_t ssdv_packets_in_image = 0;

//     flash_init();
//     uint32_t flash_id = flash_read_id();
//     if (flash_id != FLASH_ID_512) {
//         printf("Flash not found or incorrect ID: 0x%lX\r\n", flash_id);
//         while (1);
//     }

//     uint32_t image_index = 0;  // Use index 0 for new captured image
//     uint32_t ssdv_start_addr = image_index * (SSDV_PKT_SIZE * MAX_SSDV_PACKETS);

//     for (uint32_t addr = ssdv_start_addr; addr < (ssdv_start_addr + SSDV_PKT_SIZE * MAX_SSDV_PACKETS); addr += FLASH_SECTOR_SIZE) {
//         flash_erase_sector(addr);
//     }

//     uint32_t bi = 0;
//     int c;
//     while (1) {
//         while ((c = ssdv_enc_get_packet(&ssdv)) == SSDV_FEED_ME) {
//             uint32_t chunk_size = ((jpeg_length - bi) >= 128) ? 128 : (jpeg_length - bi);
//             if (chunk_size == 0) break;
//             ssdv_enc_feed(&ssdv, &image_buffer[start_index + bi], chunk_size);
//             bi += chunk_size;
//         }

//         if (c == SSDV_EOI) {
//             printf("EOI Packet.\n");
//             break;
//         } else if (c == SSDV_OK) {
//             flash_write_buffer(ssdv_pkt, ssdv_start_addr + ssdv_packets_in_image * SSDV_PKT_SIZE, SSDV_PKT_SIZE);
//             printf("| SSDV packet %02u stored at address 0x%06lX |\n", ssdv_packets_in_image + 1,
//                    (unsigned long)(ssdv_start_addr + ssdv_packets_in_image * SSDV_PKT_SIZE));
//             ssdv_packets_in_image++;
//         } else {
//             printf("[SSDV] Error or unexpected result.\n");
//             break;
//         }
//     }

//     printf("\n[READING BACK %u SSDV PACKETS FROM FLASH]\n", ssdv_packets_in_image);
//     for (uint32_t i = 0; i < ssdv_packets_in_image; i++) {
//         uint8_t buffer[SSDV_PKT_SIZE] = {0};
//         flash_read_buffer(buffer, ssdv_start_addr + i * SSDV_PKT_SIZE, SSDV_PKT_SIZE);
//         HAL_Delay(10);

//         printf("Packet %02lu: ", i + 1);
//         for (uint16_t j = 0; j < SSDV_PKT_SIZE; j++) {
//             printf("0x%02X, ", buffer[j]);
//             if ((j + 1) % 16 == 0) printf("\n");
//         }
//         printf("\n-----------------------------\n");
//     }

//     printf("[FLASH READ COMPLETE]\r\n");
// }

// void prestore_image()
// {
//     uint32_t image_index = 1;  // Reserved slot for pre-stored image
//     uint32_t static_ssdv_start_addr = image_index * (SSDV_PKT_SIZE * MAX_SSDV_PACKETS);

//     for (uint32_t addr = static_ssdv_start_addr; addr < (static_ssdv_start_addr + SSDV_PKT_SIZE * MAX_SSDV_PACKETS); addr += FLASH_SECTOR_SIZE) {
//         flash_erase_sector(addr);
//     }

//     flash_write_buffer(test_card_ssdv_buffer, static_ssdv_start_addr , TEST_CARD_SSDV_BUFFER_SIZE);

//     printf("SSDV buffer of %u bytes written to flash at address 0x%06lX\n",
//            TEST_CARD_SSDV_BUFFER_SIZE, (unsigned long)static_ssdv_start_addr);
// }

// void read_prestored_image() {
//     uint32_t image_index = 1;  // Must match write index
//     uint32_t static_ssdv_start_addr = image_index * (SSDV_PKT_SIZE * MAX_SSDV_PACKETS);

//     flash_read_buffer(test_card_ssdv_buffer, static_ssdv_start_addr, TEST_CARD_SSDV_BUFFER_SIZE);

//     printf("SSDV image read from flash at address 0x%06lX\n", (unsigned long)static_ssdv_start_addr);

//     for (int i = 0; i < TEST_CARD_SSDV_BUFFER_SIZE; i++) {
//         printf("0x%02X, ", test_card_ssdv_buffer[i]);
//         if (i != 0 && i % 224 == 0) printf("\n");
//     }
//     printf("\n");
// }



// void prestore_image()
// {

//     flash_init();

//     uint32_t flash_id = flash_read_id();
//     if (flash_id != FLASH_ID_512) {
//         printf("Flash not found or not matching (ID: 0x%lX)\r\n", flash_id);
//         while (1);
//     } else {
//         printf("Flash found (ID: 0x%lX)\r\n", flash_id);
//     }
    
//     uint32_t image_index = 1;  // Reserved slot for pre-stored image
//     uint32_t static_ssdv_start_addr = image_index * (SSDV_PKT_SIZE * MAX_SSDV_PACKETS);

//     for (uint32_t addr = static_ssdv_start_addr; addr < (static_ssdv_start_addr + SSDV_PKT_SIZE * MAX_SSDV_PACKETS); addr += FLASH_SECTOR_SIZE) {
//         flash_erase_sector(addr);
//     }

//     flash_write_buffer(test_card_ssdv_buffer, static_ssdv_start_addr , TEST_CARD_SSDV_BUFFER_SIZE);

//     printf("SSDV buffer of %u bytes written to flash at address 0x%06lX\n",
//            TEST_CARD_SSDV_BUFFER_SIZE, (unsigned long)static_ssdv_start_addr);
// }

// void read_prestored_image() {
//     uint32_t image_index = 1;  // Must match write index
//     uint32_t static_ssdv_start_addr = image_index * (SSDV_PKT_SIZE * MAX_SSDV_PACKETS);

//     flash_read_buffer(test_card_ssdv_buffer, static_ssdv_start_addr, TEST_CARD_SSDV_BUFFER_SIZE);

//     printf("SSDV image read from flash at address 0x%06lX\n", (unsigned long)static_ssdv_start_addr);

//     for (int i = 0; i < TEST_CARD_SSDV_BUFFER_SIZE; i++) {
//         printf("0x%02X, ", test_card_ssdv_buffer[i]);
//         if (i != 0 && i % 224 == 0) printf("\n");
//     }
//     printf("\n");
// }
