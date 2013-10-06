/**
 * @file counter.c
 *
 * @brief This file contains programming counter functions
 *
 * @author Thomas Fischl
 * @copyright (c) 2013 Thomas Fischl
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
#include <avr/eeprom.h>

/**
 * @brief Defines how often the counter value is stored in EEPROM
 */
#define COUNTER_REDUNCY 3

/**
 * @brief Read current programming counter value from EEPROM
 * @return Programming counter value
 */
uint16_t counter_read() {

    uint16_t counter = 0xffff;
    uint8_t i;
    uint16_t * eeadr = 0;

    for (i = 0; i < COUNTER_REDUNCY; i++) {

        uint16_t eeval = eeprom_read_word(eeadr++);

        if (eeval == ~eeprom_read_word(eeadr++)) {
            // valid value

            if (eeval < counter) counter = eeval;
        }
    }

    return counter;
}

/**
 * @brief Write given programming counter value to EEPROM
 * @param counter Programming counter value
 */
void counter_write(uint16_t counter) {

    uint8_t i;
    uint16_t * eeadr = 0;

    for (i = 0; i < COUNTER_REDUNCY; i++) {

        eeprom_write_word(eeadr++, counter);
        eeprom_write_word(eeadr++, ~counter);
    }

}

/**
 * @brief Decrement the programming counter
 * @param startvalue Initial value of programming counter
 */
void counter_decrement(uint16_t startvalue) {

    uint16_t counter = counter_read();

    if (counter == 0xffff) counter = startvalue;
    if (counter == 0) return;

    counter--;

    counter_write(counter);
}
