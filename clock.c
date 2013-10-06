/**
 * @file clock.c
 *
 * @brief This file contains clock and timing functions
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
#include <avr/io.h>
#include <avr/interrupt.h>
#include "hal.h"
#include "clock.h"

/**
 * @brief This variable ticks slow generated with timer interrupt
 */
uint8_t slowticker;


/**
 * @brief Initialize timer
 */
void clock_init() {
    
    // set timer 0 prescaler to 1/1024
    TCCR0 = (1 << CS02) | (1 << CS00);

    // reset slow ticking variable
    slowticker = 0;

    // enable timer0 interrupt
    TIMSK |= (1 << TOIE0);
}


/**
 * @brief Get current value of slow-ticker
 * @return Current slow-ticker
 */
uint8_t clock_getTickerSlow() {
    return slowticker;
}

/**
 * @brief Get difference between given ticker and current slow-ticker
 * @param ticker Ticker to compare with current slow-ticker
 * @return Difference of current ticker and given ticker
 */
uint8_t clock_getTickerSlowDiff(uint8_t ticker) {
    return (uint8_t) (clock_getTickerSlow() - ticker);
}

/**
 * @brief Wait given ticks (reference is slow-ticker)
 * @param ticks Ticks to wait
 */
void clock_delaySlow(uint8_t ticks) {
    uint8_t ticker = clock_getTickerSlow();
    while (clock_getTickerSlowDiff(ticker) < ticks) {
    };
}

/**
 * @brief Get current value of fast-ticker
 * @return 
 */
uint8_t clock_getTickerFast() {
    return TCNT0;
}

/**
 * @brief Get difference between given ticker and current fast-ticker
 * @param ticker Ticker to compare with current fast-ticker
 * @return Ticks between current and given fast-ticker
 */
uint8_t clock_getTickerFastDiff(uint8_t ticker) {
    return (uint8_t) (clock_getTickerFast() - ticker);
}

/**
 * @brief Wait given ticks (reference is fast-ticker)
 * @param ticks Ticks to wait
 */
void clock_delayFast(uint8_t ticks) {
    uint8_t ticker = clock_getTickerFast();
    while (clock_getTickerFastDiff(ticker) < ticks) {
    };
}

/**
 * @brief Timer 0 overflow interrupt
 */
ISR(TIMER0_OVF_vect) {
    slowticker++;
}
