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
#include "shared-bindings/pulseio/__init__.h"
#include "shared-bindings/pulseio/PulseIn.h"
#include "shared-bindings/pulseio/PulseOut.h"
#include "shared-bindings/pulseio/PWMOut.h"

//| :mod:`pulseio` --- Support for pulse based protocols
//| =====================================================
//|
//| .. module:: pulseio
//|   :synopsis: Support for pulse based protocols
//|   :platform: SAMD21, ESP8266
//|
//| The `pulseio` module contains classes to provide access to basic pulse IO.
//|
//| Libraries
//|
//| .. toctree::
//|     :maxdepth: 3
//|
//|     PulseIn
//|     PulseOut
//|     PWMOut
//|

//| .. warning:: This module is not available in some SAMD21 builds. See the
//|   :ref:`module-support-matrix` for more info.
//|

//| All classes change hardware state and should be deinitialized when they
//| are no longer needed if the program continues after use. To do so, either
//| call :py:meth:`!deinit` or use a context manager. See
//| :ref:`lifetime-and-contextmanagers` for more info.
//|
//| For example::
//|
//|   import pulseio
//|   import time
//|   from board import *
//|
//|   pwm = pulseio.PWMOut(D13)
//|   pwm.duty_cycle = 2 ** 15
//|   time.sleep(0.1)
//|
//| This example will initialize the the device, set
//| :py:data:`~pulseio.PWMOut.duty_cycle`, and then sleep 0.1 seconds.
//| CircuitPython will automatically turn off the PWM when it resets all
//| hardware after program completion. Use ``deinit()`` or a ``with`` statement
//| to do it yourself.
//|

STATIC const mp_rom_map_elem_t pulseio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_pulseio) },
    { MP_ROM_QSTR(MP_QSTR_PulseIn), MP_ROM_PTR(&pulseio_pulsein_type) },
    { MP_ROM_QSTR(MP_QSTR_PulseOut), MP_ROM_PTR(&pulseio_pulseout_type) },
    { MP_ROM_QSTR(MP_QSTR_PWMOut), MP_ROM_PTR(&pulseio_pwmout_type) },
};

STATIC MP_DEFINE_CONST_DICT(pulseio_module_globals, pulseio_module_globals_table);

const mp_obj_module_t pulseio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&pulseio_module_globals,
};
