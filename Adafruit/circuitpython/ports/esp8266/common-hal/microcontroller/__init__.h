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

#ifndef MICROPY_INCLUDED_ESP8266_COMMON_HAL_MICROCONTROLLER___INIT___H
#define MICROPY_INCLUDED_ESP8266_COMMON_HAL_MICROCONTROLLER___INIT___H

#include "common-hal/microcontroller/Pin.h"

extern const mcu_pin_obj_t pin_TOUT;
extern const mcu_pin_obj_t pin_XPD_DCDC;
extern const mcu_pin_obj_t pin_MTMS;
extern const mcu_pin_obj_t pin_MTDI;
extern const mcu_pin_obj_t pin_MTCK;
extern const mcu_pin_obj_t pin_MTDO;
extern const mcu_pin_obj_t pin_GPIO2;
extern const mcu_pin_obj_t pin_GPIO0;
extern const mcu_pin_obj_t pin_GPIO4;
extern const mcu_pin_obj_t pin_SD_DATA_2;
extern const mcu_pin_obj_t pin_SD_DATA_3;
extern const mcu_pin_obj_t pin_SD_CMD;
extern const mcu_pin_obj_t pin_SD_CLK;
extern const mcu_pin_obj_t pin_SD_DATA_0;
extern const mcu_pin_obj_t pin_SD_DATA_1;
extern const mcu_pin_obj_t pin_DVDD;
extern const mcu_pin_obj_t pin_U0RXD;
extern const mcu_pin_obj_t pin_U0TXD;

#endif  // MICROPY_INCLUDED_ESP8266_COMMON_HAL_MICROCONTROLLER___INIT___H
