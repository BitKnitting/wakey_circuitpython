/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Paul Sokolovsky
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

#include "esp_mphal.h"

#include "shared-bindings/multiterminal/__init__.h"
#include "shared-module/multiterminal/__init__.h"

void common_hal_multiterminal_schedule_secondary_terminal_read(mp_obj_t socket) {
    (void) socket;
    mp_hal_signal_dupterm_input();
}

mp_obj_t common_hal_multiterminal_get_secondary_terminal() {
    return shared_module_multiterminal_get_secondary_terminal();
}

void common_hal_multiterminal_set_secondary_terminal(mp_obj_t secondary_terminal) {
    shared_module_multiterminal_set_secondary_terminal(secondary_terminal);
}

void common_hal_multiterminal_clear_secondary_terminal() {
    shared_module_multiterminal_clear_secondary_terminal();
}
