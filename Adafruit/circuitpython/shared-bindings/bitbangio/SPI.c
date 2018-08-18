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
// bitbangio.SPI class.

#include <string.h>

#include "shared-bindings/bitbangio/SPI.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/util.h"

#include "lib/utils/buffer_helper.h"
#include "lib/utils/context_manager_helpers.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "supervisor/shared/translate.h"

//| .. currentmodule:: bitbangio
//|
//| :class:`SPI` -- a 3-4 wire serial protocol
//| -----------------------------------------------
//|
//| SPI is a serial protocol that has exclusive pins for data in and out of the
//| master.  It is typically faster than :py:class:`~bitbangio.I2C` because a
//| separate pin is used to control the active slave rather than a transmitted
//| address. This class only manages three of the four SPI lines: `!clock`,
//| `!MOSI`, `!MISO`. Its up to the client to manage the appropriate slave
//| select line. (This is common because multiple slaves can share the `!clock`,
//| `!MOSI` and `!MISO` lines and therefore the hardware.)
//|
//| .. class:: SPI(clock, MOSI=None, MISO=None)
//|
//|    Construct an SPI object on the given pins.
//|
//|   :param ~microcontroller.Pin clock: the pin to use for the clock.
//|   :param ~microcontroller.Pin MOSI: the Master Out Slave In pin.
//|   :param ~microcontroller.Pin MISO: the Master In Slave Out pin.
//|

// TODO(tannewt): Support LSB SPI.
STATIC mp_obj_t bitbangio_spi_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *pos_args) {
    mp_arg_check_num(n_args, n_kw, 0, MP_OBJ_FUN_ARGS_MAX, true);
    bitbangio_spi_obj_t *self = m_new_obj(bitbangio_spi_obj_t);
    self->base.type = &bitbangio_spi_type;
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, pos_args + n_args);
    enum { ARG_clock, ARG_MOSI, ARG_MISO, ARG_baudrate, ARG_polarity, ARG_phase, ARG_bits, ARG_firstbit };
    static const mp_arg_t allowed_args[] = {
       { MP_QSTR_clock, MP_ARG_REQUIRED | MP_ARG_OBJ },
       { MP_QSTR_MOSI, MP_ARG_OBJ, {.u_obj = mp_const_none} },
       { MP_QSTR_MISO, MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, &kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    assert_pin(args[ARG_clock].u_obj, false);
    assert_pin(args[ARG_MOSI].u_obj, true);
    assert_pin(args[ARG_MISO].u_obj, true);
    const mcu_pin_obj_t* clock = MP_OBJ_TO_PTR(args[ARG_clock].u_obj);
    const mcu_pin_obj_t* mosi = MP_OBJ_TO_PTR(args[ARG_MOSI].u_obj);
    const mcu_pin_obj_t* miso = MP_OBJ_TO_PTR(args[ARG_MISO].u_obj);
    shared_module_bitbangio_spi_construct(self, clock, mosi, miso);
    return (mp_obj_t)self;
}

//|   .. method:: SPI.deinit()
//|
//|      Turn off the SPI bus.
//|
STATIC mp_obj_t bitbangio_spi_obj_deinit(mp_obj_t self_in) {
    bitbangio_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    shared_module_bitbangio_spi_deinit(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(bitbangio_spi_deinit_obj, bitbangio_spi_obj_deinit);

//|   .. method:: SPI.__enter__()
//|
//|      No-op used by Context Managers.
//|
//  Provided by context manager helper.

//|   .. method:: SPI.__exit__()
//|
//|      Automatically deinitializes the hardware when exiting a context. See
//|      :ref:`lifetime-and-contextmanagers` for more info.
//|
STATIC mp_obj_t bitbangio_spi_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    shared_module_bitbangio_spi_deinit(args[0]);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(bitbangio_spi_obj___exit___obj, 4, 4, bitbangio_spi_obj___exit__);


static void check_lock(bitbangio_spi_obj_t *self) {
    if (!shared_module_bitbangio_spi_has_lock(self)) {
        mp_raise_RuntimeError(translate("Function requires lock"));
    }
}

//|   .. method:: SPI.configure(\*, baudrate=100000, polarity=0, phase=0, bits=8)
//|
//|     Configures the SPI bus. Only valid when locked.
//|
//|     :param int baudrate: the clock rate in Hertz
//|     :param int polarity: the base state of the clock line (0 or 1)
//|     :param int phase: the edge of the clock that data is captured. First (0)
//|       or second (1). Rising or falling depends on clock polarity.
//|     :param int bits: the number of bits per word
//|
STATIC mp_obj_t bitbangio_spi_configure(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_baudrate, ARG_polarity, ARG_phase, ARG_bits };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_baudrate, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 100000} },
        { MP_QSTR_polarity, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_phase, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_bits, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 8} },
    };
    bitbangio_spi_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    raise_error_if_deinited(shared_module_bitbangio_spi_deinited(self));
    check_lock(self);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    uint8_t polarity = args[ARG_polarity].u_int;
    if (polarity != 0 && polarity != 1) {
        mp_raise_ValueError(translate("Invalid polarity"));
    }
    uint8_t phase = args[ARG_phase].u_int;
    if (phase != 0 && phase != 1) {
        mp_raise_ValueError(translate("Invalid phase"));
    }
    uint8_t bits = args[ARG_bits].u_int;
    if (bits != 8 && bits != 9) {
        mp_raise_ValueError(translate("Invalid number of bits"));
    }

    shared_module_bitbangio_spi_configure(self, args[ARG_baudrate].u_int, polarity, phase, bits);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(bitbangio_spi_configure_obj, 1, bitbangio_spi_configure);

//|   .. method:: SPI.try_lock()
//|
//|     Attempts to grab the SPI lock. Returns True on success.
//|
//|     :return: True when lock has been grabbed
//|     :rtype: bool
//|
STATIC mp_obj_t bitbangio_spi_obj_try_lock(mp_obj_t self_in) {
    bitbangio_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    raise_error_if_deinited(shared_module_bitbangio_spi_deinited(self));
    return mp_obj_new_bool(shared_module_bitbangio_spi_try_lock(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(bitbangio_spi_try_lock_obj, bitbangio_spi_obj_try_lock);

//|   .. method:: SPI.unlock()
//|
//|     Releases the SPI lock.
//|
STATIC mp_obj_t bitbangio_spi_obj_unlock(mp_obj_t self_in) {
    bitbangio_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    raise_error_if_deinited(shared_module_bitbangio_spi_deinited(self));
    shared_module_bitbangio_spi_unlock(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(bitbangio_spi_unlock_obj, bitbangio_spi_obj_unlock);

//|   .. method:: SPI.write(buf)
//|
//|     Write the data contained in ``buf``. Requires the SPI being locked.
//|     If the buffer is empty, nothing happens.
//|
// TODO(tannewt): Add support for start and end kwargs.
STATIC mp_obj_t bitbangio_spi_write(mp_obj_t self_in, mp_obj_t wr_buf) {
    bitbangio_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    raise_error_if_deinited(shared_module_bitbangio_spi_deinited(self));
    mp_buffer_info_t src;
    mp_get_buffer_raise(wr_buf, &src, MP_BUFFER_READ);
    if (src.len == 0) {
        return mp_const_none;
    }
    check_lock(self);
    bool ok = shared_module_bitbangio_spi_write(self, src.buf, src.len);
    if (!ok) {
        mp_raise_OSError(MP_EIO);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(bitbangio_spi_write_obj, bitbangio_spi_write);


//|   .. method:: SPI.readinto(buf)
//|
//|     Read into the buffer specified by ``buf`` while writing zeroes.
//|     Requires the SPI being locked.
//|     If the number of bytes to read is 0, nothing happens.
//|
// TODO(tannewt): Add support for start and end kwargs.
STATIC mp_obj_t bitbangio_spi_readinto(size_t n_args, const mp_obj_t *args) {
    bitbangio_spi_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    raise_error_if_deinited(shared_module_bitbangio_spi_deinited(self));
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[1], &bufinfo, MP_BUFFER_WRITE);
    if (bufinfo.len == 0) {
        return mp_const_none;
    }
    check_lock(args[0]);
    bool ok = shared_module_bitbangio_spi_read(self, bufinfo.buf, bufinfo.len);
    if (!ok) {
        mp_raise_OSError(MP_EIO);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(bitbangio_spi_readinto_obj, 2, 2, bitbangio_spi_readinto);

//|   .. method:: SPI.write_readinto(buffer_out, buffer_in, \*, out_start=0, out_end=len(buffer_out), in_start=0, in_end=len(buffer_in))
//|
//|     Write out the data in ``buffer_out`` while simultaneously reading data into ``buffer_in``.
//|     The lengths of the slices defined by ``buffer_out[out_start:out_end]`` and ``buffer_in[in_start:in_end]``
//|     must be equal.
//|     If buffer slice lengths are both 0, nothing happens.
//|
//|     :param bytearray buffer_out: Write out the data in this buffer
//|     :param bytearray buffer_in: Read data into this buffer
//|     :param int out_start: Start of the slice of buffer_out to write out: ``buffer_out[out_start:out_end]``
//|     :param int out_end: End of the slice; this index is not included
//|     :param int in_start: Start of the slice of ``buffer_in`` to read into: ``buffer_in[in_start:in_end]``
//|     :param int in_end: End of the slice; this index is not included
//|
STATIC mp_obj_t bitbangio_spi_write_readinto(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_buffer_out, ARG_buffer_in, ARG_out_start, ARG_out_end, ARG_in_start, ARG_in_end };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_buffer_out,    MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_buffer_in,     MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_out_start,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_out_end,       MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = INT_MAX} },
        { MP_QSTR_in_start,      MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_in_end,        MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = INT_MAX} },
    };
    bitbangio_spi_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    raise_error_if_deinited(shared_module_bitbangio_spi_deinited(self));

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_buffer_info_t buf_out_info;
    mp_get_buffer_raise(args[ARG_buffer_out].u_obj, &buf_out_info, MP_BUFFER_READ);
    int32_t out_start = args[ARG_out_start].u_int;
    uint32_t out_length = buf_out_info.len;
    normalize_buffer_bounds(&out_start, args[ARG_out_end].u_int, &out_length);

    mp_buffer_info_t buf_in_info;
    mp_get_buffer_raise(args[ARG_buffer_in].u_obj, &buf_in_info, MP_BUFFER_WRITE);
    int32_t in_start = args[ARG_in_start].u_int;
    uint32_t in_length = buf_in_info.len;
    normalize_buffer_bounds(&in_start, args[ARG_in_end].u_int, &in_length);

    if (out_length != in_length) {
        mp_raise_ValueError(translate("buffer slices must be of equal length"));
    }

    if (out_length == 0) {
        return mp_const_none;
    }

    bool ok = shared_module_bitbangio_spi_transfer(self,
                                            ((uint8_t*)buf_out_info.buf) + out_start,
                                            ((uint8_t*)buf_in_info.buf) + in_start,
                                            out_length);
    if (!ok) {
        mp_raise_OSError(MP_EIO);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(bitbangio_spi_write_readinto_obj, 2, bitbangio_spi_write_readinto);

STATIC const mp_rom_map_elem_t bitbangio_spi_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&bitbangio_spi_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&bitbangio_spi_obj___exit___obj) },

    { MP_ROM_QSTR(MP_QSTR_configure), MP_ROM_PTR(&bitbangio_spi_configure_obj) },
    { MP_ROM_QSTR(MP_QSTR_try_lock), MP_ROM_PTR(&bitbangio_spi_try_lock_obj) },
    { MP_ROM_QSTR(MP_QSTR_unlock), MP_ROM_PTR(&bitbangio_spi_unlock_obj) },

    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&bitbangio_spi_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&bitbangio_spi_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_write_readinto), MP_ROM_PTR(&bitbangio_spi_write_readinto_obj) },
};
STATIC MP_DEFINE_CONST_DICT(bitbangio_spi_locals_dict, bitbangio_spi_locals_dict_table);

const mp_obj_type_t bitbangio_spi_type = {
    { &mp_type_type },
    .name = MP_QSTR_SPI,
    .make_new = bitbangio_spi_make_new,
    .locals_dict = (mp_obj_dict_t*)&bitbangio_spi_locals_dict,
};
