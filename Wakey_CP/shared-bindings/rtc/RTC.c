/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Noralf Trønnes
 * Copyright (c) 2013, 2014 Damien P. George
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

#include <string.h>

#include "py/obj.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "lib/timeutils/timeutils.h"
#include "shared-bindings/rtc/__init__.h"
#include "shared-bindings/rtc/RTC.h"
#include "shared-bindings/time/__init__.h"

void MP_WEAK common_hal_rtc_get_time(timeutils_struct_time_t *tm) {
    mp_raise_NotImplementedError("RTC is not supported on this board");
}

void MP_WEAK common_hal_rtc_set_time(timeutils_struct_time_t *tm) {
    mp_raise_NotImplementedError("RTC is not supported on this board");
}

int MP_WEAK common_hal_rtc_get_calibration(void) {
    return 0;
}

void MP_WEAK common_hal_rtc_set_calibration(int calibration) {
    mp_raise_NotImplementedError("RTC calibration is not supported on this board");
}

const rtc_rtc_obj_t rtc_rtc_obj = {{&rtc_rtc_type}};

//| .. currentmodule:: rtc
//|
//| :class:`RTC` --- Real Time Clock
//| --------------------------------
//|
//| .. class:: RTC()
//|
//|   This class represents the onboard Real Time Clock. It is a singleton and will always return the same instance.
//|
STATIC mp_obj_t rtc_rtc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // No arguments
    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    // return constant object
    return (mp_obj_t)&rtc_rtc_obj;
}

//|   .. attribute:: datetime
//|
//|       The date and time of the RTC.
//|
STATIC mp_obj_t rtc_rtc_obj_get_datetime(mp_obj_t self_in) {
    timeutils_struct_time_t tm;
    common_hal_rtc_get_time(&tm);
    return struct_time_from_tm(&tm);
}
MP_DEFINE_CONST_FUN_OBJ_1(rtc_rtc_get_datetime_obj, rtc_rtc_obj_get_datetime);

STATIC mp_obj_t rtc_rtc_obj_set_datetime(mp_obj_t self_in, mp_obj_t datetime) {
    timeutils_struct_time_t tm;
    struct_time_to_tm(datetime, &tm);
    common_hal_rtc_set_time(&tm);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(rtc_rtc_set_datetime_obj, rtc_rtc_obj_set_datetime);

const mp_obj_property_t rtc_rtc_datetime_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&rtc_rtc_get_datetime_obj,
              (mp_obj_t)&rtc_rtc_set_datetime_obj,
              (mp_obj_t)&mp_const_none_obj},
};

//|   .. attribute:: calibration
//|
//|     The RTC calibration value.
//|     A positive value speeds up the clock and a negative value slows it down.
//|     Range and value is hardware specific, but one step is often approx. 1 ppm.
//|
STATIC mp_obj_t rtc_rtc_obj_get_calibration(mp_obj_t self_in) {
    int calibration = common_hal_rtc_get_calibration();
    return mp_obj_new_int(calibration);
}
MP_DEFINE_CONST_FUN_OBJ_1(rtc_rtc_get_calibration_obj, rtc_rtc_obj_get_calibration);

STATIC mp_obj_t rtc_rtc_obj_set_calibration(mp_obj_t self_in, mp_obj_t calibration) {
    common_hal_rtc_set_calibration(mp_obj_get_int(calibration));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(rtc_rtc_set_calibration_obj, rtc_rtc_obj_set_calibration);

const mp_obj_property_t rtc_rtc_calibration_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&rtc_rtc_get_calibration_obj,
              (mp_obj_t)&rtc_rtc_set_calibration_obj,
              (mp_obj_t)&mp_const_none_obj},
};

STATIC const mp_rom_map_elem_t rtc_rtc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_datetime), MP_ROM_PTR(&rtc_rtc_datetime_obj) },
    { MP_ROM_QSTR(MP_QSTR_calibration), MP_ROM_PTR(&rtc_rtc_calibration_obj) },
};
STATIC MP_DEFINE_CONST_DICT(rtc_rtc_locals_dict, rtc_rtc_locals_dict_table);

const mp_obj_type_t rtc_rtc_type = {
    { &mp_type_type },
    .name = MP_QSTR_RTC,
    .make_new = rtc_rtc_make_new,
    .locals_dict = (mp_obj_dict_t*)&rtc_rtc_locals_dict,
};
