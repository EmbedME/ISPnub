/**
 * @file isp.h
 *
 * @brief This file contains definitions for ISP programming functions
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
#ifndef _ISP_
#define _ISP_

#define ISP_CMD_LOAD_EXTENDED_ADDRESS_BYTE 0x4D
#define ISP_CMD_LOAD_PROGRAM_MEMORY_PAGE_LOW_BYTE 0x40
#define ISP_CMD_READ_PROGRAM_MEMORY_PAGE_LOW_BYTE 0x20
#define ISP_CMD_WRITE_PROGRAM_MEMORY_PAGE 0x4C
#define ISP_CMD_LOAD_EEPROM_MEMORY_PAGE 0xC1
#define ISP_CMD_WRITE_EEPROM_MEMORY_PAGE 0xC2
#define ISP_CMD_WRITE_EEPROM_MEMORY 0xC0
#define ISP_CMD_READ_EEPROM_MEMORY 0xA0

#define ISP_DELAY_FLASH CLOCK_TICKER_FAST_5MS
#define ISP_DELAY_EEPROM CLOCK_TICKER_FAST_10MS


uint8_t isp_connect(uint8_t sckoption);
uint8_t isp_disconnect();
void isp_transmit(uint8_t * data, uint8_t len);
void isp_writeFlash(uint32_t mempointer, uint32_t address, uint32_t length, uint16_t pagesize);
uint8_t isp_verifyFlash(uint32_t mempointer, uint32_t address, uint32_t length);
void isp_writeEEPROM(uint32_t mempointer, uint32_t address, uint32_t length, uint16_t pagesize);
uint8_t isp_verifyEEPROM(uint32_t mempointer, uint32_t address, uint32_t length);

#endif
