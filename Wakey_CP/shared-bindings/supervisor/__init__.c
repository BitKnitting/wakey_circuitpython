/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2017 Scott Shawcroft for Adafruit Industries
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
 #include "py/obj.h"
 #include "py/runtime.h"
 #include "py/reload.h"

 #include "lib/utils/interrupt_char.h"
 #include "supervisor/shared/autoreload.h"

 #include "supervisor/shared/rgb_led_status.h"
 
 #include "shared-bindings/supervisor/__init__.h"
 #include "shared-bindings/supervisor/Runtime.h"

//| :mod:`supervisor` --- Supervisor settings
//| =================================================
//|
//| .. module:: supervisor
//|   :synopsis: Supervisor settings
//|   :platform: SAMD21/51 (All), nRF (Runtime only)
//|
//| The `supervisor` module. (TODO: expand description)
//|
//| Libraries
//|
//| .. toctree::
//|     :maxdepth: 3
//|
//|     Runtime
//|

//| .. attribute:: runtime
//|
//|   Runtime information, such as `runtime.serial_connected`
//|   (USB serial connection status).
//|   This object is the sole instance of `supervisor.Runtime`.
//|

//| .. method:: enable_autoreload()
//|
//|   Enable autoreload based on USB file write activity.
//|
STATIC mp_obj_t supervisor_enable_autoreload(void) {
    autoreload_enable();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(supervisor_enable_autoreload_obj, supervisor_enable_autoreload);

//| .. method:: disable_autoreload()
//|
//|   Disable autoreload based on USB file write activity until
//|   `enable_autoreload` is called.
//|
STATIC mp_obj_t supervisor_disable_autoreload(void) {
    autoreload_disable();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(supervisor_disable_autoreload_obj, supervisor_disable_autoreload);

//| .. method:: set_rgb_status_brightness()
//|
//|   Set brightness of status neopixel from 0-255
//|   `set_rgb_status_brightness` is called.
//|
STATIC mp_obj_t supervisor_set_rgb_status_brightness(mp_obj_t lvl){
      // This must be int. If cast to uint8_t first, will never raise a ValueError.
      int brightness_int = mp_obj_get_int(lvl);
      if(brightness_int < 0 || brightness_int > 255){
            mp_raise_ValueError("Brightness must be between 0 and 255");
      }
      set_rgb_status_brightness((uint8_t)brightness_int);
      return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(supervisor_set_rgb_status_brightness_obj, supervisor_set_rgb_status_brightness);

//| .. method:: reload()
//|
//|   Reload the main Python code and run it (equivalent to hitting Ctrl-D at the REPL).
//|
STATIC mp_obj_t supervisor_reload(void) {
    reload_requested = true;
    mp_raise_reload_exception();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(supervisor_reload_obj, supervisor_reload);


STATIC const mp_rom_map_elem_t supervisor_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_supervisor) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_enable_autoreload),  MP_ROM_PTR(&supervisor_enable_autoreload_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_disable_autoreload),  MP_ROM_PTR(&supervisor_disable_autoreload_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_set_rgb_status_brightness),  MP_ROM_PTR(&supervisor_set_rgb_status_brightness_obj) },
    { MP_ROM_QSTR(MP_QSTR_runtime),  MP_ROM_PTR(&common_hal_supervisor_runtime_obj) },
    { MP_ROM_QSTR(MP_QSTR_reload),  MP_ROM_PTR(&supervisor_reload_obj) },
};

STATIC MP_DEFINE_CONST_DICT(supervisor_module_globals, supervisor_module_globals_table);

const mp_obj_module_t supervisor_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&supervisor_module_globals,
};
