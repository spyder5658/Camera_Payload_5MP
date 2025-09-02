#ifndef __FLASH_H
#define __FLASH_H

#include "stm32f1xx_hal.h"  // Adjust according to your STM32 family
#include <stdint.h>

/* Flash Commands */
#define FLASH_CMD_WRITE        0x02
#define FLASH_CMD_WRSR         0x01
#define FLASH_CMD_WREN         0x06
#define FLASH_CMD_READ         0x03
#define FLASH_CMD_RDSR         0x05
#define FLASH_CMD_RDID         0x9F
#define FLASH_CMD_SE           0x20  // Sector Erase
#define FLASH_CMD_BE           0xC7  // Bulk Erase

/* Flash Constants */
#define FLASH_SPI_PAGESIZE     256   
#define FLASH_DUMMY_BYTE       0xA5
#define FLASH_WIP_FLAG         0x01  // Write In Progress flag in status register

#define FLASH_ID_512 0xEF4020


#define FLASH_ID_W25Q128     0xEF4020
#define FLASH_SECTOR_SIZE    4096
#define SSDV_PKT_SIZE        224
#define MAX_SSDV_PACKETS     45

/* Chip Select Pin Control */
void flash_cs_low(void);
void flash_cs_high(void);

/* Initialization and Control */
void flash_init(void);
void flash_deinit(void);

/* SPI Transmission */
uint8_t  flash_send_byte(uint8_t byte);
uint16_t FLASH_SendHalfWord(uint16_t HalfWord);

/* Read/Write Operations */
void flash_write_enable(void);
void flash_wait_for_write_end(void);
void flash_erase_sector(uint32_t SectorAddr);
void flash_erase_block(void);

void flash_write_page(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void flash_write_buffer(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void flash_read_buffer(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);

/* ID and Sequence Read */
uint32_t flash_read_id(void);
void flash_start_read_sequence(uint32_t ReadAddr);
uint8_t flash_read_byte(void);

#endif /* __FLASH_H */
