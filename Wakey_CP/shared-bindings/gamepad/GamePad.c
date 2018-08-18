/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Radomir Dopieralski for Adafruit Industries
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
#include "py/mphal.h"
#include "py/gc.h"
#include "py/mpstate.h"
#include "shared-module/gamepad/__init__.h"
#include "shared-module/gamepad/GamePad.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/util.h"
#include "GamePad.h"


//| .. currentmodule:: gamepad
//|
//| :class:`GamePad` -- Scan buttons for presses
//| ============================================
//|
//| Usage::
//|
//|     import board
//|     import digitalio
//|     import gamepad
//|     import time
//|
//|     B_UP = 1 << 0
//|     B_DOWN = 1 << 1
//|
//|
//|     pad = gamepad.GamePad(
//|         digitalio.DigitalInOut(board.D10),
//|         digitalio.DigitalInOut(board.D11),
//|     )
//|
//|     y = 0
//|     while True:
//|         buttons = pad.get_pressed()
//|         if buttons & B_UP:
//|             y -= 1
//|             print(y)
//|         elif buttons & B_DOWN:
//|             y += 1
//|             print(y)
//|         time.sleep(0.1)
//|         while buttons:
//|             # Wait for all buttons to be released.
//|             buttons = pad.get_pressed()
//|             time.sleep(0.1)
//|

//| .. class:: GamePad([b1[, b2[, b3[, b4[, b5[, b6[, b7[, b8]]]]]]]])
//|
//|     Initializes button scanning routines.
//|
//|     The ``b1``-``b8`` parameters are ``DigitalInOut`` objects, which
//|     immediately get switched to input with a pull-up, and then scanned
//|     regularly for button presses. The order is the same as the order of
//|     bits returned by the ``get_pressed`` function. You can re-initialize
//|     it with different keys, then the new object will replace the previous
//|     one.
//|
//|     The basic feature required here is the ability to poll the keys at
//|     regular intervals (so that de-bouncing is consistent) and fast enough
//|     (so that we don't miss short button presses) while at the same time
//|     letting the user code run normally, call blocking functions and wait
//|     on delays.
//|
//|     They button presses are accumulated, until the ``get_pressed`` method
//|     is called, at which point the button state is cleared, and the new
//|     button presses start to be recorded.
//|
STATIC mp_obj_t gamepad_make_new(const mp_obj_type_t *type, size_t n_args,
        size_t n_kw, const mp_obj_t *args) {
    if (n_args > 8) {
        mp_raise_TypeError("too many arguments");
    }
    for (size_t i = 0; i < n_args; ++i) {
        if (!MP_OBJ_IS_TYPE(args[i], &digitalio_digitalinout_type)) {
            mp_raise_TypeError("expected a DigitalInOut");
        }
        digitalio_digitalinout_obj_t *pin = MP_OBJ_TO_PTR(args[i]);
        raise_error_if_deinited(
            common_hal_digitalio_digitalinout_deinited(pin));
    }
    if (!MP_STATE_VM(gamepad_singleton)) {
        gamepad_obj_t* gamepad_singleton = m_new_obj(gamepad_obj_t);
        gamepad_singleton->base.type = &gamepad_type;
        MP_STATE_VM(gamepad_singleton) = gc_make_long_lived(gamepad_singleton);
    }
    gamepad_init(n_args, args);
    return MP_OBJ_FROM_PTR(MP_STATE_VM(gamepad_singleton));
}


//|     .. method:: get_pressed()
//|
//|         Get the status of buttons pressed since the last call and clear it.
//|
//|         Returns an 8-bit number, with bits that correspond to buttons,
//|         which have been pressed (or held down) since the last call to this
//|         function set to 1, and the remaining bits set to 0. Then it clears
//|         the button state, so that new button presses (or buttons that are
//|         held down) can be recorded for the next call.
//|
STATIC mp_obj_t gamepad_get_pressed(mp_obj_t self_in) {
    gamepad_obj_t* gamepad_singleton = MP_STATE_VM(gamepad_singleton);
    mp_obj_t gamepad = MP_OBJ_NEW_SMALL_INT(gamepad_singleton->pressed);
    gamepad_singleton->pressed = 0;
    return gamepad;
}
MP_DEFINE_CONST_FUN_OBJ_1(gamepad_get_pressed_obj, gamepad_get_pressed);


//|     .. method:: deinit()
//|
//|         Disable button scanning.
//|
STATIC mp_obj_t gamepad_deinit(mp_obj_t self_in) {
    gamepad_reset();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(gamepad_deinit_obj, gamepad_deinit);


STATIC const mp_rom_map_elem_t gamepad_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_get_pressed),  MP_ROM_PTR(&gamepad_get_pressed_obj)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_deinit),  MP_ROM_PTR(&gamepad_deinit_obj)},
};
STATIC MP_DEFINE_CONST_DICT(gamepad_locals_dict, gamepad_locals_dict_table);
const mp_obj_type_t gamepad_type = {
    { &mp_type_type },
    .name = MP_QSTR_GamePad,
    .make_new = gamepad_make_new,
    .locals_dict = (mp_obj_dict_t*)&gamepad_locals_dict,
};

