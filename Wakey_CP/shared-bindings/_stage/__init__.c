/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Radomir Dopieralski
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

#include "__init__.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-module/_stage/__init__.h"
#include "Layer.h"
#include "Text.h"

//| :mod:`_stage` --- C-level helpers for animation of sprites on a stage
//| =====================================================================
//|
//| .. module:: _stage
//|   :synopsis: C-level helpers for animation of sprites on a stage
//|   :platform: SAMD21
//|
//| The `_stage` module contains native code to speed-up the ```stage`` Library
//| <https://github.com/python-ugame/circuitpython-stage>`_.
//| Libraries
//|
//| .. toctree::
//|     :maxdepth: 3
//|
//|     Layer
//|     Text
//|
//| .. function:: render(x0, y0, x1, y1, layers, buffer, spi)
//|
//|     Render and send to the display a fragment of the screen.
//|
//|     :param int x0: Left edge of the fragment.
//|     :param int y0: Top edge of the fragment.
//|     :param int x1: Right edge of the fragment.
//|     :param int y1: Bottom edge of the fragment.
//|     :param list layers: A list of the :py:class:`~_stage.Layer` objects.
//|     :param bytearray buffer: A buffer to use for rendering.
//|     :param ~busio.SPI spi: The SPI bus to use.
//|
//|     Note that this function only sends the raw pixel data. Setting up
//|     the display for receiving it and handling the chip-select and
//|     data-command pins has to be done outside of it.
//|     There are also no sanity checks, outside of the basic overflow
//|     checking. The caller is responsible for making the passed parameters
//|     valid.
//|
//|     This function is intended for internal use in the ``stage`` library
//|     and all the necessary checks are performed there.
STATIC mp_obj_t stage_render(size_t n_args, const mp_obj_t *args) {
    uint16_t x0 = mp_obj_get_int(args[0]);
    uint16_t y0 = mp_obj_get_int(args[1]);
    uint16_t x1 = mp_obj_get_int(args[2]);
    uint16_t y1 = mp_obj_get_int(args[3]);

    size_t layers_size = 0;
    mp_obj_t *layers;
    mp_obj_get_array(args[4], &layers_size, &layers);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[5], &bufinfo, MP_BUFFER_WRITE);
    uint16_t *buffer = bufinfo.buf;
    size_t buffer_size = bufinfo.len / 2; // 16-bit indexing

    busio_spi_obj_t *spi = MP_OBJ_TO_PTR(args[6]);

    if (!render_stage(x0, y0, x1, y1, layers, layers_size,
            buffer, buffer_size, spi)) {
        mp_raise_OSError(MP_EIO);
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(stage_render_obj, 7, 7, stage_render);


STATIC const mp_rom_map_elem_t stage_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR__stage) },
    { MP_ROM_QSTR(MP_QSTR_Layer), MP_ROM_PTR(&mp_type_layer) },
    { MP_ROM_QSTR(MP_QSTR_Text), MP_ROM_PTR(&mp_type_text) },
    { MP_ROM_QSTR(MP_QSTR_render), MP_ROM_PTR(&stage_render_obj) },
};

STATIC MP_DEFINE_CONST_DICT(stage_module_globals, stage_module_globals_table);

const mp_obj_module_t stage_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&stage_module_globals,
};
