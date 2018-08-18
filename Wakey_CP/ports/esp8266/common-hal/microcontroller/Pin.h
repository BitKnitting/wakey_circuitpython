/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_ESP8266_COMMON_HAL_MICROCONTROLLER_PIN_H
#define MICROPY_INCLUDED_ESP8266_COMMON_HAL_MICROCONTROLLER_PIN_H

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    qstr name;
    uint8_t gpio_number;
    uint8_t gpio_function;
    uint32_t peripheral;
} mcu_pin_obj_t;

// Magic values for gpio_number.
#define NO_GPIO 0xff
#define SPECIAL_CASE 0xfe

void claim_pin(const mcu_pin_obj_t* pin);
void reset_pin(const mcu_pin_obj_t* pin);
void reset_pins(void);

void microcontroller_pin_register_intr_handler(uint8_t gpio_number, void (*func)(void *), void *data);
void microcontroller_pin_call_intr_handlers(uint32_t status);

#endif  // MICROPY_INCLUDED_ESP8266_COMMON_HAL_MICROCONTROLLER_PIN_H
