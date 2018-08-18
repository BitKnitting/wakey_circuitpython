/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_ATMEL_SAMD_COMMON_HAL_ROTARYIO_INCREMENTALENCODER_H
#define MICROPY_INCLUDED_ATMEL_SAMD_COMMON_HAL_ROTARYIO_INCREMENTALENCODER_H

#include "common-hal/microcontroller/Pin.h"

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    uint8_t pin_a;
    uint8_t pin_b;
    uint8_t eic_channel_a:4;
    uint8_t eic_channel_b:4;
    uint8_t last_state:4;   // <old A><old B><new A><new B>
    int8_t quarter_count:4; // count intermediate transitions between detents
    mp_int_t position;
} rotaryio_incrementalencoder_obj_t;


void incrementalencoder_interrupt_handler(uint8_t channel);

#endif // MICROPY_INCLUDED_ATMEL_SAMD_COMMON_HAL_ROTARYIO_INCREMENTALENCODER_H
