/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft
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

#ifndef MICROPY_INCLUDED_ATMEL_SAMD_COMMON_HAL_BUSIO_UART_H
#define MICROPY_INCLUDED_ATMEL_SAMD_COMMON_HAL_BUSIO_UART_H

#include "common-hal/microcontroller/Pin.h"

#include "hal/include/hal_usart_async.h"

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    struct usart_async_descriptor usart_desc;
    uint8_t rx_pin;
    uint8_t tx_pin;
    uint8_t character_bits;
    bool rx_error;
    uint32_t baudrate;
    uint32_t timeout_ms;
    // Index of the oldest received character.
    uint32_t buffer_start;
    // Index of the next available spot to store a character.
    uint32_t buffer_size;
    uint32_t buffer_length;
    uint8_t* buffer;
} busio_uart_obj_t;

#endif // MICROPY_INCLUDED_ATMEL_SAMD_COMMON_HAL_BUSIO_UART_H
