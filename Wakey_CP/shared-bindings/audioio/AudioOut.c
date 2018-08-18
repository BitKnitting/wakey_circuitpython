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
#include "shared-bindings/audioio/RawSample.h"
#include "shared-bindings/util.h"

//| .. currentmodule:: audioio
//|
//| :class:`AudioOut` -- Output an analog audio signal
//| ========================================================
//|
//| AudioOut can be used to output an analog audio signal on a given pin.
//|
//| .. class:: AudioOut(left_channel, right_channel=None)
//|
//|   Create a AudioOut object associated with the given pin(s). This allows you to
//|   play audio signals out on the given pin(s).
//|
//|   :param ~microcontroller.Pin left_channel: The pin to output the left channel to
//|   :param ~microcontroller.Pin right_channel: The pin to output the right channel to
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
//|     sine_wave = array.array("H", [0] * length)
//|     for i in range(length):
//|         sine_wave[i] = int(math.sin(math.pi * 2 * i / 18) * (2 ** 15) + 2 ** 15)
//|
//|     dac = audioio.AudioOut(board.SPEAKER)
//|     sine_wave = audioio.RawSample(sine_wave, mono=True, sample_rate=8000)
//|     dac.play(sine_wave, loop=True)
//|     time.sleep(1)
//|     sample.stop()
//|
//|   Playing a wave file from flash::
//|
//|     import board
//|     import audioio
//|     import digitalio
//|
//|     # Required for CircuitPlayground Express
//|     speaker_enable = digitalio.DigitalInOut(board.SPEAKER_ENABLE)
//|     speaker_enable.switch_to_output(value=True)
//|
//|     data = open("cplay-5.1-16bit-16khz.wav", "rb")
//|     wav = audioio.WaveFile(data)
//|     a = audioio.AudioOut(board.A0)
//|
//|     print("playing")
//|     a.play(wav)
//|     while a.playing:
//|       pass
//|     print("stopped")
//|
STATIC mp_obj_t audioio_audioout_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *pos_args) {
    mp_arg_check_num(n_args, n_kw, 1, 2, true);
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, pos_args + n_args);
    enum { ARG_left_channel, ARG_right_channel };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_left_channel, MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_right_channel, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_rom_obj = mp_const_none} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, &kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t left_channel_obj = args[ARG_left_channel].u_obj;
    assert_pin(left_channel_obj, false);
    const mcu_pin_obj_t *left_channel_pin = MP_OBJ_TO_PTR(left_channel_obj);

    mp_obj_t right_channel_obj = args[ARG_right_channel].u_obj;
    const mcu_pin_obj_t *right_channel_pin = NULL;
    if (right_channel_obj != mp_const_none) {
        assert_pin(right_channel_obj, false);
        right_channel_pin = MP_OBJ_TO_PTR(right_channel_obj);
    }

    // create AudioOut object from the given pin
    audioio_audioout_obj_t *self = m_new_obj(audioio_audioout_obj_t);
    self->base.type = &audioio_audioout_type;
    common_hal_audioio_audioout_construct(self, left_channel_pin, right_channel_pin);

    return MP_OBJ_FROM_PTR(self);
}

//|   .. method:: deinit()
//|
//|      Deinitialises the AudioOut and releases any hardware resources for reuse.
//|
STATIC mp_obj_t audioio_audioout_deinit(mp_obj_t self_in) {
    audioio_audioout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_audioio_audioout_deinit(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(audioio_audioout_deinit_obj, audioio_audioout_deinit);

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
STATIC mp_obj_t audioio_audioout_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_audioio_audioout_deinit(args[0]);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(audioio_audioout___exit___obj, 4, 4, audioio_audioout_obj___exit__);


//|   .. method:: play(sample, *, loop=False)
//|
//|     Plays the sample once when loop=False and continuously when loop=True.
//|     Does not block. Use `playing` to block.
//|
//|     Sample must be an `audioio.WaveFile` or `audioio.RawSample`.
//|
//|     The sample itself should consist of 16 bit samples. Microcontrollers with a lower output
//|     resolution will use the highest order bits to output. For example, the SAMD21 has a 10 bit
//|     DAC that ignores the lowest 6 bits when playing 16 bit samples.
//|
STATIC mp_obj_t audioio_audioout_obj_play(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_sample, ARG_loop };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_sample,    MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_loop,      MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = false} },
    };
    audioio_audioout_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    raise_error_if_deinited(common_hal_audioio_audioout_deinited(self));
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t sample = args[ARG_sample].u_obj;
    common_hal_audioio_audioout_play(self, sample, args[ARG_loop].u_bool);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(audioio_audioout_play_obj, 1, audioio_audioout_obj_play);

//|   .. method:: stop()
//|
//|     Stops playback and resets to the start of the sample.
//|
STATIC mp_obj_t audioio_audioout_obj_stop(mp_obj_t self_in) {
    audioio_audioout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    raise_error_if_deinited(common_hal_audioio_audioout_deinited(self));
    common_hal_audioio_audioout_stop(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(audioio_audioout_stop_obj, audioio_audioout_obj_stop);

//|   .. attribute:: playing
//|
//|     True when an audio sample is being output even if `paused`. (read-only)
//|
STATIC mp_obj_t audioio_audioout_obj_get_playing(mp_obj_t self_in) {
    audioio_audioout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    raise_error_if_deinited(common_hal_audioio_audioout_deinited(self));
    return mp_obj_new_bool(common_hal_audioio_audioout_get_playing(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(audioio_audioout_get_playing_obj, audioio_audioout_obj_get_playing);

const mp_obj_property_t audioio_audioout_playing_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&audioio_audioout_get_playing_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};

//|   .. method:: pause()
//|
//|     Stops playback temporarily while remembering the position. Use `resume` to resume playback.
//|
STATIC mp_obj_t audioio_audioout_obj_pause(mp_obj_t self_in) {
    audioio_audioout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    raise_error_if_deinited(common_hal_audioio_audioout_deinited(self));

    if (!common_hal_audioio_audioout_get_playing(self)) {
        mp_raise_RuntimeError("Not playing");
    }
    common_hal_audioio_audioout_pause(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(audioio_audioout_pause_obj, audioio_audioout_obj_pause);

//|   .. method:: resume()
//|
//|     Resumes sample playback after :py:func:`pause`.
//|
STATIC mp_obj_t audioio_audioout_obj_resume(mp_obj_t self_in) {
    audioio_audioout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    raise_error_if_deinited(common_hal_audioio_audioout_deinited(self));

    if (common_hal_audioio_audioout_get_paused(self)) {
        common_hal_audioio_audioout_resume(self);
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(audioio_audioout_resume_obj, audioio_audioout_obj_resume);

//|   .. attribute:: paused
//|
//|     True when playback is paused. (read-only)
//|
STATIC mp_obj_t audioio_audioout_obj_get_paused(mp_obj_t self_in) {
    audioio_audioout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    raise_error_if_deinited(common_hal_audioio_audioout_deinited(self));
    return mp_obj_new_bool(common_hal_audioio_audioout_get_paused(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(audioio_audioout_get_paused_obj, audioio_audioout_obj_get_paused);

const mp_obj_property_t audioio_audioout_paused_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&audioio_audioout_get_paused_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};

STATIC const mp_rom_map_elem_t audioio_audioout_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&audioio_audioout_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&audioio_audioout___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_play), MP_ROM_PTR(&audioio_audioout_play_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&audioio_audioout_stop_obj) },
    { MP_ROM_QSTR(MP_QSTR_pause), MP_ROM_PTR(&audioio_audioout_pause_obj) },
    { MP_ROM_QSTR(MP_QSTR_resume), MP_ROM_PTR(&audioio_audioout_resume_obj) },

    // Properties
    { MP_ROM_QSTR(MP_QSTR_playing), MP_ROM_PTR(&audioio_audioout_playing_obj) },
    { MP_ROM_QSTR(MP_QSTR_paused), MP_ROM_PTR(&audioio_audioout_paused_obj) },
};
STATIC MP_DEFINE_CONST_DICT(audioio_audioout_locals_dict, audioio_audioout_locals_dict_table);

const mp_obj_type_t audioio_audioout_type = {
    { &mp_type_type },
    .name = MP_QSTR_AudioOut,
    .make_new = audioio_audioout_make_new,
    .locals_dict = (mp_obj_dict_t*)&audioio_audioout_locals_dict,
};
