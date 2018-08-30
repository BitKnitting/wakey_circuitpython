/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef MICROPY_INCLUDED_ATMEL_SAMD_PERIPHERALS_SPI_H
#define MICROPY_INCLUDED_ATMEL_SAMD_PERIPHERALS_SPI_H

#include <stdint.h>

#include "mpconfigport.h"

void samd_peripherals_sercom_clock_init(Sercom* sercom, uint8_t sercom_index);
uint8_t samd_peripherals_get_spi_dopo(uint8_t clock_pad, uint8_t mosi_pad);
uint8_t samd_peripherals_spi_baudrate_to_baud_reg_value(const uint32_t baudrate);
uint32_t samd_peripherals_spi_baud_reg_value_to_baudrate(const uint8_t baud_reg_value);
bool samd_peripherals_valid_spi_clock_pad(uint8_t clock_pad);

Sercom* sercom_insts[SERCOM_INST_NUM];

#endif // MICROPY_INCLUDED_ATMEL_SAMD_PERIPHERALS_SPI_H
