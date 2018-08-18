/*
 * This file is part of the MicroPython project, http://micropython.org/
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

#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/audioio/__init__.h"
#include "shared-bindings/audioio/AudioOut.h"
#include "shared-bindings/audioio/WaveFile.h"

//| :mod:`audioio` --- Support for audio input and output
//| ======================================================
//|
//| .. module:: audioio
//|   :synopsis: Support for audio input and output
//|   :platform: SAMD21
//|
//| The `audioio` module contains classes to provide access to audio IO.
//|
//| Libraries
//|
//| .. toctree::
//|     :maxdepth: 3
//|
//|     AudioOut
//|     RawSample
//|     WaveFile
//|
//| All classes change hardware state and should be deinitialized when they
//| are no longer needed if the program continues after use. To do so, either
//| call :py:meth:`!deinit` or use a context manager. See
//| :ref:`lifetime-and-contextmanagers` for more info.
//|

STATIC const mp_rom_map_elem_t audioio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_audioio) },
    { MP_ROM_QSTR(MP_QSTR_AudioOut), MP_ROM_PTR(&audioio_audioout_type) },
    { MP_ROM_QSTR(MP_QSTR_RawSample), MP_ROM_PTR(&audioio_rawsample_type) },
    { MP_ROM_QSTR(MP_QSTR_WaveFile), MP_ROM_PTR(&audioio_wavefile_type) },
};

STATIC MP_DEFINE_CONST_DICT(audioio_module_globals, audioio_module_globals_table);

const mp_obj_module_t audioio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&audioio_module_globals,
};
