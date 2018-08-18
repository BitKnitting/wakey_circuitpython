/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#include "lib/utils/context_manager_helpers.h"
#include "py/binary.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/audioio/AudioOut.h"
#include "shared-bindings/util.h"

//| .. currentmodule:: audioio
//|
//| :class:`RawSample` -- A raw audio sample buffer
//| ========================================================
//|
//| An in-memory sound sample
//|
//| .. class:: RawSample(buffer, *, channel_count=1, sample_rate=8000)
//|
//|   Create a RawSample based on the given buffer of signed values. If channel_count is more than
//|   1 then each channel's samples should alternate. In other words, for a two channel buffer, the
//|   first sample will be for channel 1, the second sample will be for channel two, the third for
//|   channel 1 and so on.
//|
//|   :param array buffer: An `array.array` with samples
//|   :param int channel_count: The number of channels in the buffer
//|   :param int sample_rate: The desired playback sample rate
//|
//|   Simple 8ksps 440 Hz sin wave::
//|
//|     import audioio
//|     import board
//|     import array
//|     import time
//|     import math
//|
//|     # Generate one period of sine wav.
//|     length = 8000 // 440
//|     sine_wave = array.array("h", [0] * length)
//|     for i in range(length):
//|         sine_wave[i] = int(math.sin(math.pi * 2 * i / 18) * (2 ** 15))
//|
//|     dac = audioio.AudioOut(board.SPEAKER)
//|     sine_wave = audioio.RawSample(sine_wave)
//|     dac.play(sine_wave, loop=True)
//|     time.sleep(1)
//|     sample.stop()
//|
STATIC mp_obj_t audioio_rawsample_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *pos_args) {
    mp_arg_check_num(n_args, n_kw, 1, 2, true);
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, pos_args + n_args);
    enum { ARG_buffer, ARG_channel_count, ARG_sample_rate };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_buffer, MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_channel_count, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 1 } },
        { MP_QSTR_sample_rate, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 8000} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, &kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    audioio_rawsample_obj_t *self = m_new_obj(audioio_rawsample_obj_t);
    self->base.type = &audioio_rawsample_type;
    mp_buffer_info_t bufinfo;
    if (mp_get_buffer(args[ARG_buffer].u_obj, &bufinfo, MP_BUFFER_READ)) {
        uint8_t bytes_per_sample = 1;
        bool signed_samples = bufinfo.typecode == 'b' || bufinfo.typecode == 'h';
        if (bufinfo.typecode == 'h' || bufinfo.typecode == 'H') {
            bytes_per_sample = 2;
        } else if (bufinfo.typecode != 'b' && bufinfo.typecode != 'B' && bufinfo.typecode != BYTEARRAY_TYPECODE) {
            mp_raise_ValueError("sample_source buffer must be a bytearray or array of type 'h', 'H', 'b' or 'B'");
        }
        common_hal_audioio_rawsample_construct(self, ((uint8_t*)bufinfo.buf), bufinfo.len,
                                               bytes_per_sample, signed_samples, args[ARG_channel_count].u_int,
                                               args[ARG_sample_rate].u_int);
    } else {
        mp_raise_TypeError("buffer must be a bytes-like object");
    }

    return MP_OBJ_FROM_PTR(self);
}

//|   .. method:: deinit()
//|
//|      Deinitialises the AudioOut and releases any hardware resources for reuse.
//|
STATIC mp_obj_t audioio_rawsample_deinit(mp_obj_t self_in) {
    audioio_rawsample_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_audioio_rawsample_deinit(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(audioio_rawsample_deinit_obj, audioio_rawsample_deinit);

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
STATIC mp_obj_t audioio_rawsample_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_audioio_rawsample_deinit(args[0]);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(audioio_rawsample___exit___obj, 4, 4, audioio_rawsample_obj___exit__);

//|   .. attribute:: sample_rate
//|
//|     32 bit value that dictates how quickly samples are played in Hertz (cycles per second).
//|     When the sample is looped, this can change the pitch output without changing the underlying
//|     sample. This will not change the sample rate of any active playback. Call ``play`` again to
//|     change it.
//|
STATIC mp_obj_t audioio_rawsample_obj_get_sample_rate(mp_obj_t self_in) {
    audioio_rawsample_obj_t *self = MP_OBJ_TO_PTR(self_in);
    raise_error_if_deinited(common_hal_audioio_rawsample_deinited(self));
    return MP_OBJ_NEW_SMALL_INT(common_hal_audioio_rawsample_get_sample_rate(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(audioio_rawsample_get_sample_rate_obj, audioio_rawsample_obj_get_sample_rate);

STATIC mp_obj_t audioio_rawsample_obj_set_sample_rate(mp_obj_t self_in, mp_obj_t sample_rate) {
    audioio_rawsample_obj_t *self = MP_OBJ_TO_PTR(self_in);
    raise_error_if_deinited(common_hal_audioio_rawsample_deinited(self));
    common_hal_audioio_rawsample_set_sample_rate(self, mp_obj_get_int(sample_rate));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(audioio_rawsample_set_sample_rate_obj, audioio_rawsample_obj_set_sample_rate);

const mp_obj_property_t audioio_rawsample_sample_rate_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&audioio_rawsample_get_sample_rate_obj,
              (mp_obj_t)&audioio_rawsample_set_sample_rate_obj,
              (mp_obj_t)&mp_const_none_obj},
};

STATIC const mp_rom_map_elem_t audioio_rawsample_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&audioio_rawsample_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&audioio_rawsample___exit___obj) },

    // Properties
    { MP_ROM_QSTR(MP_QSTR_sample_rate), MP_ROM_PTR(&audioio_rawsample_sample_rate_obj) },
};
STATIC MP_DEFINE_CONST_DICT(audioio_rawsample_locals_dict, audioio_rawsample_locals_dict_table);

const mp_obj_type_t audioio_rawsample_type = {
    { &mp_type_type },
    .name = MP_QSTR_RawSample,
    .make_new = audioio_rawsample_make_new,
    .locals_dict = (mp_obj_dict_t*)&audioio_rawsample_locals_dict,
};
