/**
 * @file script.h
 *
 * @brief This file contains definitions for script processing functions
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

#ifndef _SCRIPT_
#define _SCRIPT_

/**
 * @brief Sections defintion of script data
 */
#define SCRIPT_SECTION   __attribute__((section(".script_section")))

#define SCRIPT_CMD_CONNECT      0x01    ///< Command: Connect
#define SCRIPT_CMD_DISCONNECT   0x02    ///< Command: Disconnect
#define SCRIPT_CMD_SPI_SEND     0x03    ///< Command: SPI send
#define SCRIPT_CMD_SPI_VERIFY   0x04    ///< Command: SPI verify
#define SCRIPT_CMD_FLASH        0x05    ///< Command: Flash data block
#define SCRIPT_CMD_WAIT         0x06    ///< Command: Wait x*10ms
#define SCRIPT_CMD_DECCOUNTER   0x07    ///< Command: Decrement programming counter
#define SCRIPT_CMD_EEPROM       0x08    ///< Command: Write eeprom data block
#define SCRIPT_CMD_END          0xff    ///< Command: End of script

uint8_t script_run();

#endif

