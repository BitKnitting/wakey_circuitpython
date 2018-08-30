/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#include "nrfx.h"
#include "nrfx_power.h"
#include "tick.h"
#include "usb.h"
#include "lib/utils/interrupt_char.h"
#include "lib/mp-readline/readline.h"

#ifdef SOFTDEVICE_PRESENT
#include "nrf_sdm.h"
#include "nrf_soc.h"
#endif

/* tinyusb function that handles power event (detected, ready, removed)
 * We must call it within SD's SOC event handler, or set it as power event handler if SD is not enabled.
 */
extern void tusb_hal_nrf_power_event(uint32_t event);

void usb_init(void) {

    // USB power may already be ready at this time -> no event generated
    // We need to invoke the handler based on the status initially
    uint32_t usb_reg;

#ifdef SOFTDEVICE_PRESENT
    uint8_t sd_en = false;
    (void) sd_softdevice_is_enabled(&sd_en);

    if ( sd_en ) {
        sd_power_usbdetected_enable(true);
        sd_power_usbpwrrdy_enable(true);
        sd_power_usbremoved_enable(true);

        sd_power_usbregstatus_get(&usb_reg);
    }else
#else
    {
        // Power module init
        const nrfx_power_config_t pwr_cfg = { 0 };
        nrfx_power_init(&pwr_cfg);

        // Register tusb function as USB power handler
        const nrfx_power_usbevt_config_t config = { .handler = (nrfx_power_usb_event_handler_t) tusb_hal_nrf_power_event };
        nrfx_power_usbevt_init(&config);

        nrfx_power_usbevt_enable();

        usb_reg = NRF_POWER->USBREGSTATUS;
    }
#endif

    if ( usb_reg & POWER_USBREGSTATUS_VBUSDETECT_Msk ) {
        tusb_hal_nrf_power_event(NRFX_POWER_USB_EVT_DETECTED);
    }

    if ( usb_reg & POWER_USBREGSTATUS_OUTPUTRDY_Msk ) {
        tusb_hal_nrf_power_event(NRFX_POWER_USB_EVT_READY);
    }

    tusb_init();

#if MICROPY_KBD_EXCEPTION
    // Set Ctrl+C as wanted char, tud_cdc_rx_wanted_cb() callback will be invoked when Ctrl+C is received
    // This callback always got invoked regardless of mp_interrupt_char value since we only set it once here
    tud_cdc_set_wanted_char(CHAR_CTRL_C);
#endif
}

//--------------------------------------------------------------------+
// tinyusb callbacks
//--------------------------------------------------------------------+
void tud_mount_cb(void) {
}

void tud_umount_cb(void) {
}

uint32_t tusb_hal_millis(void) {
    uint64_t ms;
    uint32_t us;
    current_tick(&ms, &us);
    return (uint32_t) ms;
}

#if MICROPY_KBD_EXCEPTION

/**
 * Callback invoked when received an "wanted" char.
 * @param itf           Interface index (for multiple cdc interfaces)
 * @param wanted_char   The wanted char (set previously)
 */
void tud_cdc_rx_wanted_cb(uint8_t itf, char wanted_char)
{
    (void) itf; // not used

    // Workaround for using lib/utils/interrupt_char.c
    // Compare mp_interrupt_char with wanted_char and ignore if not matched
    if (mp_interrupt_char == wanted_char) {
        tud_cdc_read_flush();    // flush read fifo
        mp_keyboard_interrupt();
    }
}

#endif

