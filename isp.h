/**
 * @file isp.h
 *
 * @brief This file contains definitions for ISP programming functions
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
#ifndef _ISP_
#define _ISP_

uint8_t isp_connect(uint8_t sckoption);
uint8_t isp_disconnect();
void isp_transmit(uint8_t * data, uint8_t len);
void isp_flash(uint32_t mempointer, uint32_t address, uint32_t length, uint16_t pagesize);
uint8_t isp_verify(uint32_t mempointer, uint32_t address, uint32_t length);

#endif
