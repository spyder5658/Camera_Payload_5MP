#include "flash.h"
#include "main.h"
#include "spi.h"

extern SPI_HandleTypeDef hspi1;

void flash_cs_low(void) {
    HAL_GPIO_WritePin(FLASH_CS_GPIO_Port, FLASH_CS_Pin, GPIO_PIN_RESET);
}

void flash_cs_high(void) {
    HAL_GPIO_WritePin(FLASH_CS_GPIO_Port, FLASH_CS_Pin, GPIO_PIN_SET);
}

void flash_init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // CS pin setup (manually handle)
    __HAL_RCC_GPIOA_CLK_ENABLE(); // Or your actual port

    GPIO_InitStruct.Pin = FLASH_CS_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(FLASH_CS_GPIO_Port, &GPIO_InitStruct);

    flash_cs_high(); // Deselect chip

}

void flash_deinit(void) {
    HAL_SPI_DeInit(&hspi1);
}

uint8_t flash_send_byte(uint8_t byte) {
    uint8_t received = 0;
    HAL_SPI_TransmitReceive(&hspi1, &byte, &received, 1, HAL_MAX_DELAY);
    return received;
}

uint16_t FLASH_SendHalfWord(uint16_t HalfWord) {
    uint16_t received = 0;
    HAL_SPI_TransmitReceive(&hspi1, (uint8_t*)&HalfWord, (uint8_t*)&received, 1, HAL_MAX_DELAY);
    return received;
}

void flash_write_enable(void) {
    flash_cs_low();
    flash_send_byte(FLASH_CMD_WREN);
    flash_cs_high();
}

void flash_wait_for_write_end(void) {
    uint8_t status = 0;

    flash_cs_low();
    flash_send_byte(FLASH_CMD_RDSR);

    do {
        status = flash_send_byte(FLASH_DUMMY_BYTE);
    } while ((status & FLASH_WIP_FLAG) == SET);

    flash_cs_high();
}

void flash_erase_sector(uint32_t SectorAddr) {
    flash_write_enable();
    flash_cs_low();
    flash_send_byte(FLASH_CMD_SE);
    flash_send_byte((SectorAddr & 0xFF0000) >> 16);
    flash_send_byte((SectorAddr & 0xFF00) >> 8);
    flash_send_byte(SectorAddr & 0xFF);
    flash_cs_high();
    flash_wait_for_write_end();
}

void flash_erase_block(void) {
    flash_write_enable();
    flash_cs_low();
    flash_send_byte(FLASH_CMD_BE);
    flash_cs_high();
    flash_wait_for_write_end();
}

void flash_write_page(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite) {
    flash_write_enable();
    flash_cs_low();
    flash_send_byte(FLASH_CMD_WRITE);
    flash_send_byte((WriteAddr & 0xFF0000) >> 16);
    flash_send_byte((WriteAddr & 0xFF00) >> 8);
    flash_send_byte(WriteAddr & 0xFF);

    for (uint16_t i = 0; i < NumByteToWrite; i++) {
        flash_send_byte(pBuffer[i]);
    }

    flash_cs_high();
    flash_wait_for_write_end();
}

void flash_write_buffer(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite) {
    uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

    Addr = WriteAddr % FLASH_SPI_PAGESIZE;
    count = FLASH_SPI_PAGESIZE - Addr;
    NumOfPage = NumByteToWrite / FLASH_SPI_PAGESIZE;
    NumOfSingle = NumByteToWrite % FLASH_SPI_PAGESIZE;

    if (Addr == 0) {
        if (NumOfPage == 0) {
            flash_write_page(pBuffer, WriteAddr, NumByteToWrite);
        } else {
            while (NumOfPage--) {
                flash_write_page(pBuffer, WriteAddr, FLASH_SPI_PAGESIZE);
                WriteAddr += FLASH_SPI_PAGESIZE;
                pBuffer += FLASH_SPI_PAGESIZE;
            }
            flash_write_page(pBuffer, WriteAddr, NumOfSingle);
        }
    } else {
        if (NumOfPage == 0) {
            if (NumOfSingle > count) {
                temp = NumOfSingle - count;
                flash_write_page(pBuffer, WriteAddr, count);
                WriteAddr += count;
                pBuffer += count;
                flash_write_page(pBuffer, WriteAddr, temp);
            } else {
                flash_write_page(pBuffer, WriteAddr, NumByteToWrite);
            }
        } else {
            NumByteToWrite -= count;
            NumOfPage = NumByteToWrite / FLASH_SPI_PAGESIZE;
            NumOfSingle = NumByteToWrite % FLASH_SPI_PAGESIZE;

            flash_write_page(pBuffer, WriteAddr, count);
            WriteAddr += count;
            pBuffer += count;

            while (NumOfPage--) {
                flash_write_page(pBuffer, WriteAddr, FLASH_SPI_PAGESIZE);
                WriteAddr += FLASH_SPI_PAGESIZE;
                pBuffer += FLASH_SPI_PAGESIZE;
            }

            if (NumOfSingle != 0) {
                flash_write_page(pBuffer, WriteAddr, NumOfSingle);
            }
        }
    }
}

void flash_read_buffer(uint8_t *pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead) {
    flash_cs_low();
    flash_send_byte(FLASH_CMD_READ);
    flash_send_byte((ReadAddr & 0xFF0000) >> 16);
    flash_send_byte((ReadAddr & 0xFF00) >> 8);
    flash_send_byte(ReadAddr & 0xFF);

    for (uint16_t i = 0; i < NumByteToRead; i++) {
        pBuffer[i] = flash_send_byte(FLASH_DUMMY_BYTE);
    }

    flash_cs_high();
}

uint32_t flash_read_id(void) {
    uint32_t Temp = 0;
    uint8_t Temp0, Temp1, Temp2;

    flash_cs_low();
    flash_send_byte(0x9F);
    Temp0 = flash_send_byte(FLASH_DUMMY_BYTE);
    Temp1 = flash_send_byte(FLASH_DUMMY_BYTE);
    Temp2 = flash_send_byte(FLASH_DUMMY_BYTE);
    flash_cs_high();

    Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;
    return Temp;
}

void flash_start_read_sequence(uint32_t ReadAddr) {
    flash_cs_low();
    flash_send_byte(FLASH_CMD_READ);
    flash_send_byte((ReadAddr & 0xFF0000) >> 16);
    flash_send_byte((ReadAddr & 0xFF00) >> 8);
    flash_send_byte(ReadAddr & 0xFF);
}

uint8_t flash_read_byte(void) {
    return flash_send_byte(FLASH_DUMMY_BYTE);
}
