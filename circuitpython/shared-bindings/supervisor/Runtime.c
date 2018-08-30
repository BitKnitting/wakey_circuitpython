/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Michael Schroeder
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

#include <stdbool.h>
#include "py/objproperty.h"
#include "shared-bindings/supervisor/Runtime.h"

//TODO: add USB, REPL to description once they're operational
//| .. currentmodule:: supervisor
//|
//| :class:`Runtime` --- Supervisor Runtime information
//| ----------------------------------------------------
//|
//| Get current status of runtime objects.
//|
//| Usage::
//|
//|    import supervisor
//|    if supervisor.runtime.serial_connected:
//|        print("Hello World!")
//|

//| .. class:: Runtime()
//|
//|     You cannot create an instance of `supervisor.Runtime`.
//|     Use `supervisor.runtime` to access the sole instance available.
//|

//|     .. attribute:: runtime.serial_connected
//|
//|         Returns the USB serial communication status (read-only).
//|
//|     .. note::
//|
//|         SAMD: Will return ``True`` if the USB serial connection
//|         has been established at any point.  Will not reset if
//|         USB is disconnected but power remains (e.g. battery connected)
//|
//|         Feather52 (nRF52832): Currently returns ``True`` regardless
//|         of USB connection status.
//|

STATIC mp_obj_t supervisor_get_serial_connected(mp_obj_t self){
    if (!common_hal_get_serial_connected()) {
        return mp_const_false;
    }
    else {
        return mp_const_true;
    }
}
MP_DEFINE_CONST_FUN_OBJ_1(supervisor_get_serial_connected_obj, supervisor_get_serial_connected);

const mp_obj_property_t supervisor_serial_connected_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&supervisor_get_serial_connected_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};

STATIC const mp_rom_map_elem_t supervisor_runtime_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_serial_connected), MP_ROM_PTR(&supervisor_serial_connected_obj) },
};

STATIC MP_DEFINE_CONST_DICT(supervisor_runtime_locals_dict, supervisor_runtime_locals_dict_table);

const mp_obj_type_t supervisor_runtime_type = {
    .base = { &mp_type_type },
    .name = MP_QSTR_Runtime,
    .locals_dict = (mp_obj_t)&supervisor_runtime_locals_dict,
};
