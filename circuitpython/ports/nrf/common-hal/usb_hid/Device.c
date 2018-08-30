/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 hathach for Adafruit Industries
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
#include "tick.h"
#include "common-hal/usb_hid/Device.h"
#include "py/runtime.h"
#include "shared-bindings/usb_hid/Device.h"
#include "supervisor/shared/translate.h"
#include "tusb.h"

uint8_t common_hal_usb_hid_device_get_usage_page(usb_hid_device_obj_t *self) {
    return self->usage_page;
}

uint8_t common_hal_usb_hid_device_get_usage(usb_hid_device_obj_t *self) {
    return self->usage;
}

void common_hal_usb_hid_device_send_report(usb_hid_device_obj_t *self, uint8_t* report, uint8_t len) {
    if (len != self->report_length) {
        mp_raise_ValueError_varg(translate("Buffer incorrect size. Should be %d bytes."), self->report_length);
    }

    // Wait until interface is ready, timeout = 2 seconds
    uint64_t end_ticks = ticks_ms + 2000;
    while ( (ticks_ms < end_ticks) && !tud_hid_generic_ready() ) { }

    if ( !tud_hid_generic_ready() ) {
        mp_raise_msg(&mp_type_OSError,  translate("USB Busy"));
    }

    memcpy(self->report_buffer, report, len);

    if ( !tud_hid_generic_report(self->report_id, self->report_buffer, len) ) {
        mp_raise_msg(&mp_type_OSError, translate("USB Error"));
    }
}

// Callbacks invoked when receive Get_Report request through control endpoint
uint16_t tud_hid_generic_get_report_cb(uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen) {
    // only support Input Report
    if ( report_type != HID_REPORT_TYPE_INPUT ) return 0;

    // index is ID-1
    uint8_t idx =  ( report_id ? (report_id-1) : 0 );

    // fill buffer with current report
    memcpy(buffer, usb_hid_devices[idx].report_buffer, reqlen);
    return reqlen;
}

// Callbacks invoked when receive Set_Report request through control endpoint
void tud_hid_generic_set_report_cb(uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize) {
    // index is ID-1
    uint8_t idx =  ( report_id ? (report_id-1) : 0 );

    if ( report_type == HID_REPORT_TYPE_OUTPUT ) {
        // Check if it is Keyboard device
        if ( (usb_hid_devices[idx].usage_page == HID_USAGE_PAGE_DESKTOP) && (usb_hid_devices[idx].usage == HID_USAGE_DESKTOP_KEYBOARD) ) {
            // This is LED indicator (CapsLock, NumLock)
            // TODO Light up some LED here
        }
    }
}
