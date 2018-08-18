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

#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/busio/__init__.h"
#include "shared-bindings/busio/I2C.h"
#include "shared-bindings/busio/OneWire.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/busio/UART.h"
#include "shared-bindings/busio/__init__.h"

#include "py/runtime.h"

//| :mod:`busio` --- Hardware accelerated behavior
//| =================================================
//|
//| .. module:: busio
//|   :synopsis: Hardware accelerated behavior
//|   :platform: SAMD21
//|
//| The `busio` module contains classes to support a variety of serial
//| protocols.
//|
//| When the microcontroller does not support the behavior in a hardware
//| accelerated fashion it may internally use a bitbang routine. However, if
//| hardware support is available on a subset of pins but not those provided,
//| then a RuntimeError will be raised. Use the `bitbangio` module to explicitly
//| bitbang a serial protocol on any general purpose pins.
//|
//| Libraries
//|
//| .. toctree::
//|     :maxdepth: 3
//|
//|     I2C
//|     OneWire
//|     SPI
//|     UART
//|
//| All classes change hardware state and should be deinitialized when they
//| are no longer needed if the program continues after use. To do so, either
//| call :py:meth:`!deinit` or use a context manager. See
//| :ref:`lifetime-and-contextmanagers` for more info.
//|
//| For example::
//|
//|   import busio
//|   from board import *
//|
//|   i2c = busio.I2C(SCL, SDA)
//|   print(i2c.scan())
//|   i2c.deinit()
//|
//| This example will initialize the the device, run
//| :py:meth:`~busio.I2C.scan` and then :py:meth:`~busio.I2C.deinit` the
//| hardware. The last step is optional because CircuitPython automatically
//| resets hardware after a program finishes.
//|

STATIC const mp_rom_map_elem_t busio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_busio) },
    { MP_ROM_QSTR(MP_QSTR_I2C),   MP_ROM_PTR(&busio_i2c_type) },
    { MP_ROM_QSTR(MP_QSTR_SPI),   MP_ROM_PTR(&busio_spi_type) },
    { MP_ROM_QSTR(MP_QSTR_OneWire),   MP_ROM_PTR(&busio_onewire_type) },
    { MP_ROM_QSTR(MP_QSTR_UART),   MP_ROM_PTR(&busio_uart_type) },
};

STATIC MP_DEFINE_CONST_DICT(busio_module_globals, busio_module_globals_table);

const mp_obj_module_t busio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&busio_module_globals,
};
