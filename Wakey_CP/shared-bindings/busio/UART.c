/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#include "shared-bindings/busio/UART.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/util.h"

#include "lib/utils/context_manager_helpers.h"

#include "py/ioctl.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "py/stream.h"


//| .. currentmodule:: busio
//|
//| :class:`UART` -- a bidirectional serial protocol
//| =================================================
//|
//|
//| .. class:: UART(tx, rx, \*, baudrate=9600, bits=8, parity=None, stop=1, timeout=1000, receiver_buffer_size=64)
//|
//|   A common bidirectional serial protocol that uses an an agreed upon speed
//|   rather than a shared clock line.
//|
//|   :param ~microcontroller.Pin tx: the pin to transmit with, or ``None`` if this ``UART`` is receive-only.
//|   :param ~microcontroller.Pin rx: the pin to receive on, or ``None`` if this ``UART`` is transmit-only.
//|   :param int baudrate: the transmit and receive speed.
///   :param int bits:  the number of bits per byte, 7, 8 or 9.
///   :param Parity parity:  the parity used for error checking.
///   :param int stop:  the number of stop bits, 1 or 2.
///   :param int timeout:  the timeout in milliseconds to wait for the first character and between subsequent characters.
///   :param int receiver_buffer_size: the character length of the read buffer (0 to disable). (When a character is 9 bits the buffer will be 2 * receiver_buffer_size bytes.)
//|
typedef struct {
    mp_obj_base_t base;
} busio_uart_parity_obj_t;
extern const busio_uart_parity_obj_t busio_uart_parity_even_obj;
extern const busio_uart_parity_obj_t busio_uart_parity_odd_obj;

STATIC mp_obj_t busio_uart_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *pos_args) {
    mp_arg_check_num(n_args, n_kw, 0, MP_OBJ_FUN_ARGS_MAX, true);
    // Always initially allocate the UART object within the long-lived heap.
    // This is needed to avoid crashes with certain UART implementations which
    // cannot accomodate being moved after creation. (See
    // https://github.com/adafruit/circuitpython/issues/1056)
    busio_uart_obj_t *self = m_new_ll_obj(busio_uart_obj_t);
    self->base.type = &busio_uart_type;
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, pos_args + n_args);
    enum { ARG_tx, ARG_rx, ARG_baudrate, ARG_bits, ARG_parity, ARG_stop, ARG_timeout, ARG_receiver_buffer_size};
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_tx, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_rx, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_baudrate, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 9600} },
        { MP_QSTR_bits, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 8} },
        { MP_QSTR_parity, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_stop, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1000} },
        { MP_QSTR_receiver_buffer_size, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 64} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, &kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    assert_pin(args[ARG_rx].u_obj, true);
    const mcu_pin_obj_t* rx = MP_OBJ_TO_PTR(args[ARG_rx].u_obj);
    assert_pin_free(rx);

    assert_pin(args[ARG_tx].u_obj, true);
    const mcu_pin_obj_t* tx = MP_OBJ_TO_PTR(args[ARG_tx].u_obj);
    assert_pin_free(tx);

    uint8_t bits = args[ARG_bits].u_int;
    if (bits < 7 || bits > 9) {
        mp_raise_ValueError("bits must be 7, 8 or 9");
    }

    uart_parity_t parity = PARITY_NONE;
    if (args[ARG_parity].u_obj == &busio_uart_parity_even_obj) {
        parity = PARITY_EVEN;
    } else if (args[ARG_parity].u_obj == &busio_uart_parity_odd_obj) {
        parity = PARITY_ODD;
    }

    uint8_t stop = args[ARG_stop].u_int;
    if (stop != 1 && stop != 2) {
        mp_raise_ValueError("stop must be 1 or 2");
    }

    common_hal_busio_uart_construct(self, tx, rx,
        args[ARG_baudrate].u_int, bits, parity, stop, args[ARG_timeout].u_int,
        args[ARG_receiver_buffer_size].u_int);
    return (mp_obj_t)self;
}

//|   .. method:: deinit()
//|
//|      Deinitialises the UART and releases any hardware resources for reuse.
//|
STATIC mp_obj_t busio_uart_obj_deinit(mp_obj_t self_in) {
    busio_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_busio_uart_deinit(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(busio_uart_deinit_obj, busio_uart_obj_deinit);

//|   .. method:: __enter__()
//|
//|      No-op used by Context Managers.
//|
//  Provided by context manager helper.

//|   .. method:: __exit__()
//|
//|      Automatically deinitializes the hardware when exiting a context. See
//|      :ref:`lifetime-and-contextmanagers` for more info.
//|
STATIC mp_obj_t busio_uart_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_busio_uart_deinit(args[0]);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(busio_uart___exit___obj, 4, 4, busio_uart_obj___exit__);

// These are standard stream methods. Code is in py/stream.c.
//
//|   .. method:: read(nbytes=None)
//|
//|     Read characters.  If ``nbytes`` is specified then read at most that many
//|     bytes. Otherwise, read everything that arrives until the connection
//|     times out. Providing the number of bytes expected is highly recommended
//|     because it will be faster.
//|
//|     :return: Data read
//|     :rtype: bytes or None
//|
//|   .. method:: readinto(buf, nbytes=None)
//|
//|     Read bytes into the ``buf``.  If ``nbytes`` is specified then read at most
//|     that many bytes.  Otherwise, read at most ``len(buf)`` bytes.
//|
//|     :return: number of bytes read and stored into ``buf``
//|     :rtype: bytes or None
//|
//|   .. method:: readline()
//|
//|     Read a line, ending in a newline character.
//|
//|     :return: the line read
//|     :rtype: int or None
//|
//|   .. method:: write(buf)
//|
//|     Write the buffer of bytes to the bus.
//|
//|     :return: the number of bytes written
//|     :rtype: int or None
//|

// These three methods are used by the shared stream methods.
STATIC mp_uint_t busio_uart_read(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode) {
    busio_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    raise_error_if_deinited(common_hal_busio_uart_deinited(self));
    byte *buf = buf_in;

    // make sure we want at least 1 char
    if (size == 0) {
        return 0;
    }

    return common_hal_busio_uart_read(self, buf, size, errcode);
}

STATIC mp_uint_t busio_uart_write(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) {
    busio_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    raise_error_if_deinited(common_hal_busio_uart_deinited(self));
    const byte *buf = buf_in;

    return common_hal_busio_uart_write(self, buf, size, errcode);
}

STATIC mp_uint_t busio_uart_ioctl(mp_obj_t self_in, mp_uint_t request, mp_uint_t arg, int *errcode) {
    busio_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    raise_error_if_deinited(common_hal_busio_uart_deinited(self));
    mp_uint_t ret;
    if (request == MP_IOCTL_POLL) {
        mp_uint_t flags = arg;
        ret = 0;
        if ((flags & MP_IOCTL_POLL_RD) && common_hal_busio_uart_rx_characters_available(self) > 0) {
            ret |= MP_IOCTL_POLL_RD;
        }
        if ((flags & MP_IOCTL_POLL_WR) && common_hal_busio_uart_ready_to_tx(self)) {
            ret |= MP_IOCTL_POLL_WR;
        }
    } else {
        *errcode = MP_EINVAL;
        ret = MP_STREAM_ERROR;
    }
    return ret;
}

//|   .. attribute:: baudrate
//|
//|     The current baudrate.
//|
STATIC mp_obj_t busio_uart_obj_get_baudrate(mp_obj_t self_in) {
    busio_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    raise_error_if_deinited(common_hal_busio_uart_deinited(self));
    return MP_OBJ_NEW_SMALL_INT(common_hal_busio_uart_get_baudrate(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(busio_uart_get_baudrate_obj, busio_uart_obj_get_baudrate);

STATIC mp_obj_t busio_uart_obj_set_baudrate(mp_obj_t self_in, mp_obj_t baudrate) {
    busio_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    raise_error_if_deinited(common_hal_busio_uart_deinited(self));
    common_hal_busio_uart_set_baudrate(self, mp_obj_get_int(baudrate));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(busio_uart_set_baudrate_obj, busio_uart_obj_set_baudrate);


const mp_obj_property_t busio_uart_baudrate_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&busio_uart_get_baudrate_obj,
              (mp_obj_t)&busio_uart_set_baudrate_obj,
              (mp_obj_t)&mp_const_none_obj},
};

//| .. class:: busio.UART.Parity
//|
//|     Enum-like class to define the parity used to verify correct data transfer.
//|
//|     .. data:: ODD
//|
//|       Total number of ones should be odd.
//|
//|     .. data:: EVEN
//|
//|       Total number of ones should be even.
//|
const mp_obj_type_t busio_uart_parity_type;

const busio_uart_parity_obj_t busio_uart_parity_odd_obj = {
    { &busio_uart_parity_type },
};

const busio_uart_parity_obj_t busio_uart_parity_even_obj = {
    { &busio_uart_parity_type },
};

STATIC const mp_rom_map_elem_t busio_uart_parity_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_ODD),    MP_ROM_PTR(&busio_uart_parity_odd_obj) },
    { MP_ROM_QSTR(MP_QSTR_EVEN),   MP_ROM_PTR(&busio_uart_parity_even_obj) },
};
STATIC MP_DEFINE_CONST_DICT(busio_uart_parity_locals_dict, busio_uart_parity_locals_dict_table);

STATIC void busio_uart_parity_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    qstr parity = MP_QSTR_ODD;
    if (MP_OBJ_TO_PTR(self_in) == MP_ROM_PTR(&busio_uart_parity_even_obj)) {
        parity = MP_QSTR_EVEN;
    }
    mp_printf(print, "%q.%q.%q.%q", MP_QSTR_busio, MP_QSTR_UART, MP_QSTR_Parity, parity);
}

const mp_obj_type_t busio_uart_parity_type = {
    { &mp_type_type },
    .name = MP_QSTR_Parity,
    .print = busio_uart_parity_print,
    .locals_dict = (mp_obj_t)&busio_uart_parity_locals_dict,
};

STATIC const mp_rom_map_elem_t busio_uart_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit),       MP_ROM_PTR(&busio_uart_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__),    MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__),     MP_ROM_PTR(&busio_uart___exit___obj) },

    // Standard stream methods.
    { MP_OBJ_NEW_QSTR(MP_QSTR_read),     MP_ROM_PTR(&mp_stream_read_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_readline), MP_ROM_PTR(&mp_stream_unbuffered_readline_obj)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_stream_readinto_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_write),    MP_ROM_PTR(&mp_stream_write_obj) },

    // Properties
    { MP_ROM_QSTR(MP_QSTR_baudrate), MP_ROM_PTR(&busio_uart_baudrate_obj) },
    
    // Nested Enum-like Classes.
    { MP_ROM_QSTR(MP_QSTR_Parity),       MP_ROM_PTR(&busio_uart_parity_type) },
};
STATIC MP_DEFINE_CONST_DICT(busio_uart_locals_dict, busio_uart_locals_dict_table);

STATIC const mp_stream_p_t uart_stream_p = {
    .read = busio_uart_read,
    .write = busio_uart_write,
    .ioctl = busio_uart_ioctl,
    .is_text = false,
};

const mp_obj_type_t busio_uart_type = {
    { &mp_type_type },
    .name = MP_QSTR_UART,
    .make_new = busio_uart_make_new,
    .getiter = mp_identity_getiter,
    .iternext = mp_stream_unbuffered_iter,
    .protocol = &uart_stream_p,
    .locals_dict = (mp_obj_dict_t*)&busio_uart_locals_dict,
};
