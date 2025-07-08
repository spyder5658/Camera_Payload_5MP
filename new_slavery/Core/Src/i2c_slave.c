#include "main.h"
#include "i2c_slave.h"
#include "camera.h"

volatile uint8_t capture_requested = 0;

volatile uint8_t send_all_packets = 0;
volatile uint16_t current_packet_index = 0;


extern uint32_t ssdv_start_addr;
extern uint32_t ssdv_packets_in_image;

extern I2C_HandleTypeDef hi2c2;
#define RxSize 1
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

    if (cmd == 0x10) {
        printf("Capture command received\r\n");
        capture_requested = 1;
    }
    else if (cmd == 0x40) {
        printf("SSDV stream requested\r\n");
        send_all_packets = 1;
        current_packet_index = 0;
    }
}



void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
    HAL_I2C_EnableListen_IT(hi2c);
}



