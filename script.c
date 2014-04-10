/**
 * @file script.c
 *
 * @brief This file contains script processing functions
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
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "clock.h"
#include "hal.h"
#include "isp.h"
#include "counter.h"
#include "script.h"

/**
 * @brief Pointer to script data in flash memory
 */
unsigned char scriptdata[] SCRIPT_SECTION = {SCRIPT_CMD_END}; // dummy (is overwritten by hex creator)

/**
 * @brief Execute script stored in flash memory
 * @retval 1 Everything okay
 * @retval 0 Error occured
 */
uint8_t script_run() {

    DEFINE_DATAPOINTER;

    uint8_t cmd;
    while (1) {
        cmd = flash_readbyte(scriptdata_p++);

        uint8_t success = 0;

        switch (cmd) {

            case SCRIPT_CMD_CONNECT:
                success = isp_connect(flash_readbyte(scriptdata_p++));
                break;

            case SCRIPT_CMD_DISCONNECT:
                success = isp_disconnect();
                break;

            case SCRIPT_CMD_WAIT:
            {
                uint8_t loops = flash_readbyte(scriptdata_p++);
                while (loops--) {
                    clock_delayFast(CLOCK_TICKER_FAST_10MS);
                }
                success = 1;
            }
                break;

            case SCRIPT_CMD_SPI_SEND:
            {
                uint8_t data[4];
                uint8_t i;
                for (i = 0; i < 4; i++)
                    data[i] = flash_readbyte(scriptdata_p++);
                isp_transmit(data, sizeof (data));
                success = 1;
            }
                break;

            case SCRIPT_CMD_SPI_VERIFY:
            {
                uint8_t data[4];
                uint8_t i;
                uint8_t verifybyte;

                for (i = 0; i < 4; i++)
                    data[i] = flash_readbyte(scriptdata_p++);

                verifybyte = flash_readbyte(scriptdata_p++);

                isp_transmit(data, sizeof (data));

                if (data[3] == verifybyte) success = 1;
                else success = 0;
            }
                break;
            case SCRIPT_CMD_FLASH:
            case SCRIPT_CMD_EEPROM:
            {

                uint32_t address = (uint32_t) flash_readbyte(scriptdata_p++) << 24;
                address |= (uint32_t) flash_readbyte(scriptdata_p++) << 16;
                address |= (uint32_t) flash_readbyte(scriptdata_p++) << 8;
                address |= (uint32_t) flash_readbyte(scriptdata_p++);

                uint32_t length = (uint32_t) flash_readbyte(scriptdata_p++) << 24;
                length |= (uint32_t) flash_readbyte(scriptdata_p++) << 16;
                length |= (uint32_t) flash_readbyte(scriptdata_p++) << 8;
                length |= (uint32_t) flash_readbyte(scriptdata_p++);

                uint16_t pagesize = (uint16_t) flash_readbyte(scriptdata_p++) << 8;
                pagesize |= (uint16_t) flash_readbyte(scriptdata_p++);

                if (cmd == SCRIPT_CMD_FLASH) {
                    isp_writeFlash(scriptdata_p, address, length, pagesize);
                    success = isp_verifyFlash(scriptdata_p, address, length);
                } else {
                    isp_writeEEPROM(scriptdata_p, address, length, pagesize);
                    success = isp_verifyEEPROM(scriptdata_p, address, length);
                }

                scriptdata_p += length;
            }
                break;

            case SCRIPT_CMD_DECCOUNTER:
            {
                uint16_t startvalue = (uint16_t) flash_readbyte(scriptdata_p++) << 8;
                startvalue |= (uint16_t) flash_readbyte(scriptdata_p++);
                counter_decrement(startvalue);
                success = 1;
            }
                break;

            case SCRIPT_CMD_END:
                return 1;
                break;
        }

        if (!success) {
            isp_disconnect();
            return 0;
        }
    }
}

