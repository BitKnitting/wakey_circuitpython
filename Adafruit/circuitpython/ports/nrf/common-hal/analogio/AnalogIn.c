/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
 * Copyright (c) 2018 Artur Pacholec
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

#include "common-hal/analogio/AnalogIn.h"
#include "py/runtime.h"
#include "supervisor/shared/translate.h"

#include "nrfx_saadc.h"
#include "nrf_gpio.h"

#define CHANNEL_NO 0

void common_hal_analogio_analogin_construct(analogio_analogin_obj_t *self, const mcu_pin_obj_t *pin) {
    if (pin->adc_channel == 0)
        mp_raise_ValueError(translate("Pin does not have ADC capabilities"));

    nrf_gpio_cfg_default(NRF_GPIO_PIN_MAP(pin->port, pin->pin));

    self->pin = pin;
}

bool common_hal_analogio_analogin_deinited(analogio_analogin_obj_t *self) {
    return self->pin == mp_const_none;
}

void common_hal_analogio_analogin_deinit(analogio_analogin_obj_t *self) {
    if (common_hal_analogio_analogin_deinited(self))
        return;

    nrf_gpio_cfg_default(NRF_GPIO_PIN_MAP(self->pin->port, self->pin->pin));

    self->pin = mp_const_none;
}

uint16_t common_hal_analogio_analogin_get_value(analogio_analogin_obj_t *self) {
    // Something else might have used the ADC in a different way,
    // so we completely re-initialize it.

    nrf_saadc_value_t value;

    const nrf_saadc_channel_config_t config = {
        .resistor_p = NRF_SAADC_RESISTOR_DISABLED,
        .resistor_n = NRF_SAADC_RESISTOR_DISABLED,
        .gain = NRF_SAADC_GAIN1_6,
        .reference = NRF_SAADC_REFERENCE_INTERNAL,
        .acq_time = NRF_SAADC_ACQTIME_3US,
        .mode = NRF_SAADC_MODE_SINGLE_ENDED,
        .burst = NRF_SAADC_BURST_DISABLED,
        .pin_p = self->pin->adc_channel,
        .pin_n = self->pin->adc_channel,
    };

    nrf_saadc_resolution_set(NRF_SAADC_RESOLUTION_14BIT);
    nrf_saadc_oversample_set(NRF_SAADC_OVERSAMPLE_DISABLED);
    nrf_saadc_enable();

    for (uint32_t i = 0; i < NRF_SAADC_CHANNEL_COUNT; i++)
        nrf_saadc_channel_input_set(i, NRF_SAADC_INPUT_DISABLED, NRF_SAADC_INPUT_DISABLED);

    nrf_saadc_channel_init(CHANNEL_NO, &config);
    nrf_saadc_buffer_init(&value, 1);

    nrf_saadc_task_trigger(NRF_SAADC_TASK_START);
    while (nrf_saadc_event_check(NRF_SAADC_EVENT_STARTED) == 0);
    nrf_saadc_event_clear(NRF_SAADC_EVENT_STARTED);

    nrf_saadc_task_trigger(NRF_SAADC_TASK_SAMPLE);
    while (nrf_saadc_event_check(NRF_SAADC_EVENT_END) == 0);
    nrf_saadc_event_clear(NRF_SAADC_EVENT_END);

    nrf_saadc_task_trigger(NRF_SAADC_TASK_STOP);
    while (nrf_saadc_event_check(NRF_SAADC_EVENT_STOPPED) == 0);
    nrf_saadc_event_clear(NRF_SAADC_EVENT_STOPPED);

    nrf_saadc_disable();

    if (value < 0)
        value = 0;

    // Map value to from 14 to 16 bits
    return (value << 2);
}

float common_hal_analogio_analogin_get_reference_voltage(analogio_analogin_obj_t *self) {
    return 3.3f;
}
