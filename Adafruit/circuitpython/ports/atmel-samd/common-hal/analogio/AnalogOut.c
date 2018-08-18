/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
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

#include <stdint.h>
#include <string.h>

#include "py/mperrno.h"
#include "py/runtime.h"

#include "shared-bindings/analogio/AnalogOut.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "supervisor/shared/translate.h"

#include "atmel_start_pins.h"
#include "hal/include/hal_dac_sync.h"
#include "hpl/gclk/hpl_gclk_base.h"
#include "peripheral_clk_config.h"

#ifdef SAMD21
#include "hpl/pm/hpl_pm_base.h"
#endif

void common_hal_analogio_analogout_construct(analogio_analogout_obj_t* self,
        const mcu_pin_obj_t *pin) {
    #if defined(SAMD21) && !defined(PIN_PA02)
    mp_raise_NotImplementedError(translate("No DAC on chip"));
    #else
    if (pin->number != PIN_PA02
    #ifdef SAMD51
        && pin->number != PIN_PA05
    #endif
    ) {
        mp_raise_ValueError(translate("AnalogOut not supported on given pin"));
        return;
    }

    self->channel = 0;
    #ifdef SAMD51
    if (pin->number == PIN_PA05) {
        self->channel = 1;
    }
    #endif

    #ifdef SAMD51
    hri_mclk_set_APBDMASK_DAC_bit(MCLK);
    #endif

    #ifdef SAMD21
    _pm_enable_bus_clock(PM_BUS_APBC, DAC);
    #endif

    // SAMD21: This clock should be <= 12 MHz, per datasheet section 47.6.3.
    // SAMD51: This clock should be <= 350kHz, per datasheet table 37-6.
    _gclk_enable_channel(DAC_GCLK_ID, CONF_GCLK_DAC_SRC);

    // Don't double init the DAC on the SAMD51 when both outputs are in use. We use the free state
    // of each output pin to determine DAC state.
    int32_t result = ERR_NONE;
    #ifdef SAMD51
    if (!common_hal_mcu_pin_is_free(&pin_PA02) || !common_hal_mcu_pin_is_free(&pin_PA05)) {
    #endif
        // Fake the descriptor if the DAC is already initialized.
        self->descriptor.device.hw = DAC;
    #ifdef SAMD51
    } else {
    #endif
        result = dac_sync_init(&self->descriptor, DAC);
    #ifdef SAMD51
    }
    #endif
    if (result != ERR_NONE) {
        mp_raise_OSError(MP_EIO);
        return;
    }
    claim_pin(pin);

    gpio_set_pin_function(pin->number, GPIO_PIN_FUNCTION_B);

    dac_sync_enable_channel(&self->descriptor, self->channel);
    #endif
}

bool common_hal_analogio_analogout_deinited(analogio_analogout_obj_t *self) {
    return self->deinited;
}

void common_hal_analogio_analogout_deinit(analogio_analogout_obj_t *self) {
    #if (defined(SAMD21) && defined(PIN_PA02)) || defined(SAMD51)
    if (common_hal_analogio_analogout_deinited(self)) {
        return;
    }
    dac_sync_disable_channel(&self->descriptor, self->channel);
    reset_pin(PIN_PA02);
    // Only deinit the DAC on the SAMD51 if both outputs are free.
    #ifdef SAMD51
    if (common_hal_mcu_pin_is_free(&pin_PA02) && common_hal_mcu_pin_is_free(&pin_PA05)) {
    #endif
        dac_sync_deinit(&self->descriptor);
    #ifdef SAMD51
    }
    #endif
    self->deinited = true;
    // TODO(tannewt): Turn off the DAC clocks to save power.
    #endif
}

void common_hal_analogio_analogout_set_value(analogio_analogout_obj_t *self,
        uint16_t value) {
    #if defined(SAMD21) && !defined(PIN_PA02)
    return;
    #endif
    // Input is 16 bit so make sure and set LEFTADJ to 1 so it takes the top
    // bits. This is currently done in asf4_conf/*/hpl_dac_config.h.
    dac_sync_write(&self->descriptor, self->channel, &value, 1);
}

void analogout_reset(void) {
    #if defined(SAMD21) && !defined(PIN_PA02)
    return;
    #endif
    #ifdef SAMD21
    while (DAC->STATUS.reg & DAC_STATUS_SYNCBUSY) {}
    #endif
    #ifdef SAMD51
    while (DAC->SYNCBUSY.reg & DAC_SYNCBUSY_SWRST) {}
    #endif
    DAC->CTRLA.reg |= DAC_CTRLA_SWRST;

    // TODO(tannewt): Turn off the DAC clocks to save power.
}
