/**
 * @file isp.c
 *
 * @brief This file contains ISP programming functions
 *
 * @author Thomas Fischl
 * @copyright (c) 2013-2014 Thomas Fischl
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 * 
 */
#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "clock.h"
#include "hal.h"
#include "isp.h"

/**
 * @brief Transmit given byte over SPI
 * @param send_byte Byte to transmit
 * @return Byte read from SPI during transfer
 */
uint8_t ispTransmit_hw(uint8_t send_byte) {
    SPDR = send_byte;

    while (!(SPSR & (1 << SPIF)));
    return SPDR;
}

/**
 * @brief Connect to ISP target
 * @param sckoption Programming clock option
 * @retval 0 Error occured
 * @retval 1 Connected successfully to target
 */
uint8_t isp_connect(uint8_t sckoption) {

    /* all ISP pins are inputs before */
    /* now set output pins */
    ISP_DDR |= (1 << ISP_RST) | (1 << ISP_SCK) | (1 << ISP_MOSI);

    /* reset device */
    ISP_OUT &= ~(1 << ISP_RST); /* RST low */
    ISP_OUT &= ~(1 << ISP_SCK); /* SCK low */


    uint8_t retries = 32;
    do {
        /* positive reset pulse > 2 SCK (target) */
        clock_delayFast(CLOCK_TICKER_FAST_5MS);
        ISP_OUT |= (1 << ISP_RST); /* RST high */
        clock_delayFast(CLOCK_TICKER_FAST_5MS);
        ISP_OUT &= ~(1 << ISP_RST); /* RST low */

        // wait minimum 20ms
        clock_delayFast(CLOCK_TICKER_FAST_25MS);

        // set spi clock and enable spi
        SPSR = (sckoption >> 2) & 1;
        SPCR = (1 << SPE) | (1 << MSTR) | (sckoption & 0x03);

        uint8_t data[4] = {0xAC, 0x53, 0x00, 0x00};
        isp_transmit(data, sizeof (data));

        if (data[2] == 0x53) {
            // we are in sync
            return 1;
        }

        // disable spi
        SPCR = 0;

        retries--;
    } while (retries > 0);

    return 0;
}

/**
 * @brief Disconnect from ISP target
 * @retval 1 Everything okay
 * @retval 0 Error occured
 */
uint8_t isp_disconnect() {

    // set all ISP pins inputs
    ISP_DDR &= ~((1 << ISP_RST) | (1 << ISP_SCK) | (1 << ISP_MOSI));
    // switch pullups off
    ISP_OUT &= ~((1 << ISP_RST) | (1 << ISP_SCK) | (1 << ISP_MOSI));

    // disable spi
    SPCR = 0;

    return 1;
}

/**
 * @brief Transmit given data
 * @param data Pointer to data to transmit
 * @param len Length of data block
 */
void isp_transmit(uint8_t * data, uint8_t len) {
    uint8_t i;
    for (i = 0; i < len; i++) {
        SPDR = data[i];
        while (!(SPSR & (1 << SPIF)));
        data[i] = SPDR;
    }
}

/**
 * @brief Transfer given memory block to ISP target flash
 * @param mempointer Pointer to start of data to transfer
 * @param address Target address
 * @param length Length of data block
 * @param pagesize Size of target flash page
 */
void isp_writeFlash(uint32_t mempointer, uint32_t address, uint32_t length, uint16_t pagesize) {

    uint8_t doflush = 0;
    uint8_t data[4];
    uint8_t hiaddress = 0xff;
    while (length > 0) {

        // load extended address
        if ((address >> 17) != hiaddress) {
            hiaddress = address >> 17;
            data[0] = ISP_CMD_LOAD_EXTENDED_ADDRESS_BYTE;
            data[1] = 0;
            data[2] = hiaddress;
            data[3] = 0;
            isp_transmit(data, sizeof (data));
        }

        // load byte
        data[0] = ISP_CMD_LOAD_PROGRAM_MEMORY_PAGE_LOW_BYTE | ((address & 1) << 3);
        data[1] = address >> 9;
        data[2] = address >> 1;
        data[3] = flash_readbyte(mempointer++);
        isp_transmit(data, sizeof (data));
        doflush = 1;

        if ((((address + 1) % pagesize) == 0) || ((length == 1) && (doflush))) {
            // flush page

            data[0] = ISP_CMD_WRITE_PROGRAM_MEMORY_PAGE;
            data[1] = address >> 9;
            data[2] = address >> 1;
            data[3] = 0;
            isp_transmit(data, sizeof (data));

            clock_delayFast(ISP_DELAY_FLASH);

            doflush = 0;
        }

        address++;
        length--;

    }
}

/**
 * @brief Read data from target and verify its content with given flash block
 * @param mempointer Pointer to data block to verify with
 * @param address Address of target
 * @param length Length of data block to verify
 * @retval 0 Verification error
 * @retval 1 Verification successful
 */
uint8_t isp_verifyFlash(uint32_t mempointer, uint32_t address, uint32_t length) {
    uint8_t data[4];
    uint8_t hiaddress = 0xff;
    while (length > 0) {

        // load extended address
        if ((address >> 17) != hiaddress) {
            hiaddress = address >> 17;
            data[0] = ISP_CMD_LOAD_EXTENDED_ADDRESS_BYTE;
            data[1] = 0;
            data[2] = hiaddress;
            data[3] = 0;
            isp_transmit(data, sizeof (data));
        }

        // read byte
        data[0] = ISP_CMD_READ_PROGRAM_MEMORY_PAGE_LOW_BYTE | ((address & 1) << 3);
        data[1] = address >> 9;
        data[2] = address >> 1;
        data[3] = 0;
        isp_transmit(data, sizeof (data));

        if (flash_readbyte(mempointer++) != data[3]) return 0;

        address++;
        length--;

    }
    return 1;
}

/**
 * @brief Transfer given memory block to ISP target eeprom
 * @param mempointer Pointer to start of data to transfer
 * @param address Target address
 * @param length Length of data block
 * @param pagesize Size of target flash page
 */
void isp_writeEEPROM(uint32_t mempointer, uint32_t address, uint32_t length, uint16_t pagesize) {

    uint8_t doflush = 0;
    uint8_t data[4];
    while (length > 0) {

        // load byte
        data[1] = address >> 8;
        data[2] = address & 0xff;
        data[3] = flash_readbyte(mempointer++);

        if (pagesize <= 1) {
            
            // single byte programming
            data[0] = ISP_CMD_WRITE_EEPROM_MEMORY;
            isp_transmit(data, sizeof (data));
            clock_delayFast(ISP_DELAY_EEPROM);
            
        } else {
            
            // page programming
            data[0] = ISP_CMD_LOAD_EEPROM_MEMORY_PAGE;
            isp_transmit(data, sizeof (data));
            doflush = 1;

            if ((((address + 1) % pagesize) == 0) || ((length == 1) && (doflush))) {
                // flush page

                data[0] = ISP_CMD_WRITE_EEPROM_MEMORY_PAGE;
                data[1] = address >> 8;
                data[2] = address & 0xfc;
                data[3] = 0;
                isp_transmit(data, sizeof (data));

                clock_delayFast(ISP_DELAY_EEPROM);

                doflush = 0;
            }
        }

        address++;
        length--;

    }
}

/**
 * @brief Read data from target and verify its content with given eeprom block
 * @param mempointer Pointer to data block to verify with
 * @param address Address of target
 * @param length Length of data block to verify
 * @retval 0 Verification error
 * @retval 1 Verification successful
 */
uint8_t isp_verifyEEPROM(uint32_t mempointer, uint32_t address, uint32_t length) {
    uint8_t data[4];
    while (length > 0) {

        // read byte
        data[0] = ISP_CMD_READ_EEPROM_MEMORY;
        data[1] = address >> 8;
        data[2] = address & 0xff;
        data[3] = 0;
        isp_transmit(data, sizeof (data));

        if (flash_readbyte(mempointer++) != data[3]) return 0;

        address++;
        length--;

    }
    return 1;
}