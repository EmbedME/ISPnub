/**
 * @file clock.h
 *
 * @brief This file contains definitons for clock and timing functions
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

#ifndef CLOCK_H
#define CLOCK_H

// 8 MHz / 1024 / 256 = 30.52 Hz
#define CLOCK_TICKER_SLOW_1S 31     ///< 1s slow ticks
#define CLOCK_TICKER_SLOW_500MS 15  ///< 500ms slow ticks
#define CLOCK_TICKER_SLOW_250MS 8   ///< 250ms slow ticks
#define CLOCK_TICKER_SLOW_100MS 3   ///< 100ms slow ticks

// 8 MHz / 1024 = 7812.5 Hz
#define CLOCK_TICKER_FAST_1MS 8     ///< 1ms fast ticks
#define CLOCK_TICKER_FAST_5MS 39    ///< 5ms fast ticks
#define CLOCK_TICKER_FAST_10MS 78   ///< 10ms fast ticks
#define CLOCK_TICKER_FAST_20MS 156  ///< 20ms fast ticks
#define CLOCK_TICKER_FAST_25MS 195  ///< 25ms fast ticks

void clock_init();
uint8_t clock_getTickerSlow();
uint8_t clock_getTickerSlowDiff(uint8_t ticker);
void clock_delaySlow(uint8_t ticks);
uint8_t clock_getTickerFast();
uint8_t clock_getTickerFastDiff(uint8_t ticker);
void clock_delayFast(uint8_t ticks);

#endif
