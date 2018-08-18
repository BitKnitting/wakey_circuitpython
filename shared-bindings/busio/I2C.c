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

// This file contains all of the Python API definitions for the
// busio.I2C class.

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/busio/I2C.h"
#include "shared-bindings/util.h"

#include "lib/utils/buffer_helper.h"
#include "lib/utils/context_manager_helpers.h"
#include "py/runtime.h"
#include "supervisor/shared/translate.h"

//| .. currentmodule:: busio
//|
//| :class:`I2C` --- Two wire serial protocol
//| ------------------------------------------
//|
//| .. class:: I2C(scl, sda, \*, frequency=400000)
//|
//|   I2C is a two-wire protocol for communicating between devices.  At the
//|   physical level it consists of 2 wires: SCL and SDA, the clock and data
//|   lines respectively.
//|
//|   .. seealso:: Using this class directly requires careful lock management.
//|       Instead, use :class:`~adafruit_bus_device.i2c_device.I2CDevice` to
//|       manage locks.
//|
//|   .. seealso:: Using this class to directly read registers requires manual
//|       bit unpacking. Instead, use an existing driver or make one with
//|       :ref:`Register <register-module-reference>` data descriptors.
//|
//|   :param ~microcontroller.Pin scl: The clock pin
//|   :param ~microcontroller.Pin sda: The data pin
//|   :param int frequency: The clock frequency in Hertz
//|   :param int timeout: The maximum clock stretching timeut - only for bitbang
//|
STATIC mp_obj_t busio_i2c_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *pos_args) {
    mp_arg_check_num(n_args, n_kw, 0, MP_OBJ_FUN_ARGS_MAX, true);
    busio_i2c_obj_t *self = m_new_obj(busio_i2c_obj_t);
    self->base.type = &busio_i2c_type;
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, pos_args + n_args);
    enum { ARG_scl, ARG_sda, ARG_frequency, ARG_timeout };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_scl, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_sda, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_frequency, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 400000} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 255} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, &kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    assert_pin(args[ARG_scl].u_obj, false);
    assert_pin(args[ARG_sda].u_obj, false);
    const mcu_pin_obj_t* scl = MP_OBJ_TO_PTR(args[ARG_scl].u_obj);
    assert_pin_free(scl);
    const mcu_pin_obj_t* sda = MP_OBJ_TO_PTR(args[ARG_sda].u_obj);
    assert_pin_free(sda);
    common_hal_busio_i2c_construct(self, scl, sda, args[ARG_frequency].u_int, args[ARG_timeout].u_int);
    return (mp_obj_t)self;
}

//|   .. method:: I2C.deinit()
//|
//|     Releases control of the underlying hardware so other classes can use it.
//|
STATIC mp_obj_t busio_i2c_obj_deinit(mp_obj_t self_in) {
    busio_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_busio_i2c_deinit(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(busio_i2c_deinit_obj, busio_i2c_obj_deinit);

//|   .. method:: I2C.__enter__()
//|
//|     No-op used in Context Managers.
//|
//  Provided by context manager helper.

//|   .. method:: I2C.__exit__()
//|
//|     Automatically deinitializes the hardware on context exit. See
//|     :ref:`lifetime-and-contextmanagers` for more info.
//|
STATIC mp_obj_t busio_i2c_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_busio_i2c_deinit(args[0]);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(busio_i2c___exit___obj, 4, 4, busio_i2c_obj___exit__);

static void check_lock(busio_i2c_obj_t *self) {
    asm("");
    if (!common_hal_busio_i2c_has_lock(self)) {
        mp_raise_RuntimeError(translate("Function requires lock."));
    }
}

//|   .. method:: I2C.scan()
//|
//|      Scan all I2C addresses between 0x08 and 0x77 inclusive and return a
//|      list of those that respond.
//|
//|      :return: List of device ids on the I2C bus
//|      :rtype: list
//|
STATIC mp_obj_t busio_i2c_scan(mp_obj_t self_in) {
    busio_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    raise_error_if_deinited(common_hal_busio_i2c_deinited(self));
    check_lock(self);
    mp_obj_t list = mp_obj_new_list(0, NULL);
    // 7-bit addresses 0b0000xxx and 0b1111xxx are reserved
    for (int addr = 0x08; addr < 0x78; ++addr) {
        bool success = common_hal_busio_i2c_probe(self, addr);
        if (success) {
            mp_obj_list_append(list, MP_OBJ_NEW_SMALL_INT(addr));
        }
    }
    return list;
}
MP_DEFINE_CONST_FUN_OBJ_1(busio_i2c_scan_obj, busio_i2c_scan);

//|   .. method:: I2C.try_lock()
//|
//|     Attempts to grab the I2C lock. Returns True on success.
//|
//|     :return: True when lock has been grabbed
//|     :rtype: bool
//|
STATIC mp_obj_t busio_i2c_obj_try_lock(mp_obj_t self_in) {
    busio_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    raise_error_if_deinited(common_hal_busio_i2c_deinited(self));
    return mp_obj_new_bool(common_hal_busio_i2c_try_lock(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(busio_i2c_try_lock_obj, busio_i2c_obj_try_lock);

//|   .. method:: I2C.unlock()
//|
//|     Releases the I2C lock.
//|
STATIC mp_obj_t busio_i2c_obj_unlock(mp_obj_t self_in) {
    busio_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    raise_error_if_deinited(common_hal_busio_i2c_deinited(self));
    common_hal_busio_i2c_unlock(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(busio_i2c_unlock_obj, busio_i2c_obj_unlock);

//|   .. method:: I2C.readfrom_into(address, buffer, \*, start=0, end=len(buffer))
//|
//|      Read into ``buffer`` from the slave specified by ``address``.
//|      The number of bytes read will be the length of ``buffer``.
//|      At least one byte must be read.
//|
//|      If ``start`` or ``end`` is provided, then the buffer will be sliced
//|      as if ``buffer[start:end]``. This will not cause an allocation like
//|      ``buf[start:end]`` will so it saves memory.
//|
//|      :param int address: 7-bit device address
//|      :param bytearray buffer: buffer to write into
//|      :param int start: Index to start writing at
//|      :param int end: Index to write up to but not include
//|
STATIC mp_obj_t busio_i2c_readfrom_into(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_address, ARG_buffer, ARG_start, ARG_end };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_address,    MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_buffer,     MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_start,      MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_end,        MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = INT_MAX} },
    };
    busio_i2c_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    raise_error_if_deinited(common_hal_busio_i2c_deinited(self));
    check_lock(self);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_buffer].u_obj, &bufinfo, MP_BUFFER_WRITE);

    int32_t start = args[ARG_start].u_int;
    uint32_t length = bufinfo.len;
    normalize_buffer_bounds(&start, args[ARG_end].u_int, &length);
    if (length == 0) {
        mp_raise_ValueError(translate("Buffer must be at least length 1"));
    }

    uint8_t status = common_hal_busio_i2c_read(self, args[ARG_address].u_int, ((uint8_t*)bufinfo.buf) + start, length);
    if (status != 0) {
        mp_raise_OSError(status);
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(busio_i2c_readfrom_into_obj, 3, busio_i2c_readfrom_into);

//|   .. method:: I2C.writeto(address, buffer, \*, start=0, end=len(buffer), stop=True)
//|
//|      Write the bytes from ``buffer`` to the slave specified by ``address``.
//|      Transmits a stop bit if ``stop`` is set.
//|
//|      If ``start`` or ``end`` is provided, then the buffer will be sliced
//|      as if ``buffer[start:end]``. This will not cause an allocation like
//|      ``buffer[start:end]`` will so it saves memory.
//|
//|      Writing a buffer or slice of length zero is permitted, as it can be used
//|      to poll for the existence of a device.
//|
//|      :param int address: 7-bit device address
//|      :param bytearray buffer: buffer containing the bytes to write
//|      :param int start: Index to start writing from
//|      :param int end: Index to read up to but not include
//|      :param bool stop: If true, output an I2C stop condition after the
//|                        buffer is written
//|
STATIC mp_obj_t busio_i2c_writeto(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_address, ARG_buffer, ARG_start, ARG_end, ARG_stop };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_address,    MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_buffer,     MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_start,      MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_end,        MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = INT_MAX} },
        { MP_QSTR_stop,       MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true} },
    };
    busio_i2c_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    raise_error_if_deinited(common_hal_busio_i2c_deinited(self));
    check_lock(self);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // get the buffer to write the data from
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_buffer].u_obj, &bufinfo, MP_BUFFER_READ);


    int32_t start = args[ARG_start].u_int;
    uint32_t length = bufinfo.len;
    normalize_buffer_bounds(&start, args[ARG_end].u_int, &length);

    // do the transfer
    uint8_t status = common_hal_busio_i2c_write(self, args[ARG_address].u_int,
        ((uint8_t*) bufinfo.buf) + start, length, args[ARG_stop].u_bool);
    if (status != 0) {
        mp_raise_OSError(status);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(busio_i2c_writeto_obj, 1, busio_i2c_writeto);

STATIC const mp_rom_map_elem_t busio_i2c_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&busio_i2c_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&busio_i2c___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_scan), MP_ROM_PTR(&busio_i2c_scan_obj) },

    { MP_ROM_QSTR(MP_QSTR_try_lock), MP_ROM_PTR(&busio_i2c_try_lock_obj) },
    { MP_ROM_QSTR(MP_QSTR_unlock), MP_ROM_PTR(&busio_i2c_unlock_obj) },

    { MP_ROM_QSTR(MP_QSTR_readfrom_into), MP_ROM_PTR(&busio_i2c_readfrom_into_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeto), MP_ROM_PTR(&busio_i2c_writeto_obj) },
};

STATIC MP_DEFINE_CONST_DICT(busio_i2c_locals_dict, busio_i2c_locals_dict_table);

const mp_obj_type_t busio_i2c_type = {
   { &mp_type_type },
   .name = MP_QSTR_I2C,
   .make_new = busio_i2c_make_new,
   .locals_dict = (mp_obj_dict_t*)&busio_i2c_locals_dict,
};
