/**
 * @mainpage
 * ISPnub firmware project
 *
 * @section description Description
 * ISPnub is a stand-alone AVR programming module. The programming instructions
 * are defined within scripts. These scripts are converted with an additional
 * tool (ISPnubCreator) into binary information and are stored in flash. This
 * firmware interprets this binary programming instructions.
 * 
 * The firmware hex file is packed into the JAR file of ISPnubCreator which
 * merges the firmware hex data with programming instructions from scripts.
 * 
 * Environment:
 * - Target: ATmega1284P
 * - Compiler: avr-gcc (GCC) 4.7.2
 * 
 * @section history Change History
 * - v1.0 (2013-06-14)
 *   - Initial release
 * - v1.1 (2013-10-06)
 *   - Code cleanup and documentation
 * - v1.2 (2014-04-08)
 *   - Added EEPROM programming
 *
 */

/**
 * @file main.c
 *
 * @brief This file contains the main routine with application entry point for
 *        the ISPnub firmware project
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
#include "isp.h"
#include "counter.h"
#include "hal.h"
#include "script.h"


/**
 * @brief Main routine of firmware and application entry point
 * @return Application return code
 */
int main(void) {

    hal_init();
    clock_init();

    // enable interrupts
    sei();

    uint8_t ticker = clock_getTickerSlow();
    uint8_t toggle = 0;
    uint16_t counter = counter_read();
    uint8_t success = 1;
    uint8_t keyticker = clock_getTickerSlow();
    uint8_t keylocked = 1;

    hal_setLEDgreen(1);
    hal_setLEDred(0);

    // main loop	
    while (1) {

        if (keylocked) {

            // do debouncing
            
            if (hal_getSwitch()) {
                keyticker = clock_getTickerSlow();
            } else if (clock_getTickerSlowDiff(keyticker) > CLOCK_TICKER_SLOW_500MS) {
                keylocked = 0;
            }

        } else if (hal_getSwitch()) {

            // key pressed
            
            if (counter > 0) {
                hal_setLEDgreen(1);
                hal_setLEDred(1);

                success = script_run();
                counter = counter_read();

                hal_setLEDgreen(success);
                hal_setLEDred(0);

                ticker = clock_getTickerSlow();
            } else {
                success = 0;
            }
            
            keylocked = 1;
            keyticker = clock_getTickerSlow();

        }

        // do led signaling
        if (clock_getTickerSlowDiff(ticker) > CLOCK_TICKER_SLOW_250MS) {
            ticker = clock_getTickerSlow();
            toggle = !toggle;

            if (counter == 0) hal_setLEDgreen(toggle);
            else hal_setLEDgreen(success);

            if (!success) hal_setLEDred(toggle);
            else hal_setLEDred(0);

        }
    }

    return (0);
}

