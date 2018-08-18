/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
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

#include "shared-bindings/audioio/WaveFile.h"

#include <stdint.h>
#include <string.h>

#include "py/mperrno.h"
#include "py/runtime.h"

#include "shared-module/audioio/WaveFile.h"

struct wave_format_chunk {
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
    uint16_t extra_params; // Assumed to be zero below.
};

void common_hal_audioio_wavefile_construct(audioio_wavefile_obj_t* self,
                                           pyb_file_obj_t* file) {
    // Load the wave
    self->file = file;
    uint8_t chunk_header[16];
    f_rewind(&self->file->fp);
    UINT bytes_read;
    if (f_read(&self->file->fp, chunk_header, 16, &bytes_read) != FR_OK) {
        mp_raise_OSError(MP_EIO);
    }
    if (bytes_read != 16 ||
        memcmp(chunk_header, "RIFF", 4) != 0 ||
        memcmp(chunk_header + 8, "WAVEfmt ", 8) != 0) {
        mp_raise_ValueError("Invalid wave file");
    }
    uint32_t format_size;
    if (f_read(&self->file->fp, &format_size, 4, &bytes_read) != FR_OK) {
        mp_raise_OSError(MP_EIO);
    }
    if (bytes_read != 4 ||
        format_size > sizeof(struct wave_format_chunk)) {
        mp_raise_ValueError("Invalid format chunk size");
    }
    struct wave_format_chunk format;
    if (f_read(&self->file->fp, &format, format_size, &bytes_read) != FR_OK) {
        mp_raise_OSError(MP_EIO);
    }
    if (bytes_read != format_size) {
    }

    if (format.audio_format != 1 ||
        format.num_channels > 2 ||
        format.bits_per_sample > 16 ||
        (format_size == 18 &&
         format.extra_params != 0)) {
        mp_raise_ValueError("Unsupported format");
    }
    // Get the sample_rate
    self->sample_rate = format.sample_rate;
    self->len = 512;
    self->channel_count = format.num_channels;
    self->bits_per_sample = format.bits_per_sample;

    // TODO(tannewt): Skip any extra chunks that occur before the data section.

    uint8_t data_tag[4];
    if (f_read(&self->file->fp, &data_tag, 4, &bytes_read) != FR_OK) {
        mp_raise_OSError(MP_EIO);
    }
    if (bytes_read != 4 ||
        memcmp((uint8_t *) data_tag, "data", 4) != 0) {
        mp_raise_ValueError("Data chunk must follow fmt chunk");
    }

    uint32_t data_length;
    if (f_read(&self->file->fp, &data_length, 4, &bytes_read) != FR_OK) {
        mp_raise_OSError(MP_EIO);
    }
    if (bytes_read != 4) {
        mp_raise_ValueError("Invalid file");
    }
    self->file_length = data_length;
    self->data_start = self->file->fp.fptr;

    // Try to allocate two buffers, one will be loaded from file and the other
    // DMAed to DAC.
    self->buffer = m_malloc(self->len, false);
    if (self->buffer == NULL) {
        common_hal_audioio_wavefile_deinit(self);
        mp_raise_msg(&mp_type_MemoryError, "");
    }

    self->second_buffer = m_malloc(self->len, false);
    if (self->second_buffer == NULL) {
        common_hal_audioio_wavefile_deinit(self);
        mp_raise_msg(&mp_type_MemoryError, "");
    }
}

void common_hal_audioio_wavefile_deinit(audioio_wavefile_obj_t* self) {
    self->buffer = NULL;
}

bool common_hal_audioio_wavefile_deinited(audioio_wavefile_obj_t* self) {
    return self->buffer == NULL;
}

uint32_t common_hal_audioio_wavefile_get_sample_rate(audioio_wavefile_obj_t* self) {
    return self->sample_rate;
}

void common_hal_audioio_wavefile_set_sample_rate(audioio_wavefile_obj_t* self,
                                                 uint32_t sample_rate) {
    self->sample_rate = sample_rate;
}

bool audioio_wavefile_samples_signed(audioio_wavefile_obj_t* self) {
    return self->bits_per_sample > 8;
}

uint32_t audioio_wavefile_max_buffer_length(audioio_wavefile_obj_t* self) {
    return 512;
}

void audioio_wavefile_reset_buffer(audioio_wavefile_obj_t* self,
                                   bool single_channel,
                                   uint8_t channel) {
    if (single_channel && channel == 1) {
        return;
    }
    // We don't reset the buffer index in case we're looping and we have an odd number of buffer
    // loads
    self->bytes_remaining = self->file_length;
    f_lseek(&self->file->fp, self->data_start);
    self->read_count = 0;
    self->left_read_count = 0;
    self->right_read_count = 0;
}

audioio_get_buffer_result_t audioio_wavefile_get_buffer(audioio_wavefile_obj_t* self,
                                                        bool single_channel,
                                                        uint8_t channel,
                                                        uint8_t** buffer,
                                                        uint32_t* buffer_length) {
    if (!single_channel) {
        channel = 0;
    }

    uint32_t channel_read_count = self->left_read_count;
    if (channel == 1) {
        channel_read_count = self->right_read_count;
    }

    bool need_more_data = self->read_count == channel_read_count;

    if (self->bytes_remaining == 0 && need_more_data) {
        *buffer = NULL;
        *buffer_length = 0;
        return GET_BUFFER_DONE;
    }

    if (need_more_data) {
        uint16_t num_bytes_to_load = self->len;
        if (num_bytes_to_load > self->bytes_remaining) {
            num_bytes_to_load = self->bytes_remaining;
        }
        UINT length_read;
        if (self->buffer_index % 2 == 1) {
            *buffer = self->second_buffer;
        } else {
            *buffer = self->buffer;
        }
        if (f_read(&self->file->fp, *buffer, num_bytes_to_load, &length_read) != FR_OK) {
            return GET_BUFFER_ERROR;
        }
        *buffer_length = length_read;
        if (self->buffer_index % 2 == 1) {
            self->second_buffer_length = length_read;
        } else {
            self->buffer_length = length_read;
        }
        self->bytes_remaining -= length_read;
        self->buffer_index += 1;
        self->read_count += 1;
    }

    uint32_t buffers_back = self->read_count - 1 - channel_read_count;
    if ((self->buffer_index - buffers_back) % 2 == 0) {
        *buffer = self->second_buffer;
        *buffer_length = self->second_buffer_length;
    } else {
        *buffer = self->buffer;
        *buffer_length = self->buffer_length;
    }

    if (channel == 0) {
        self->left_read_count += 1;
    } else if (channel == 1) {
        self->right_read_count += 1;
        *buffer = *buffer + self->bits_per_sample / 8;
    }

    return self->bytes_remaining == 0 ? GET_BUFFER_DONE : GET_BUFFER_MORE_DATA;
}

void audioio_wavefile_get_buffer_structure(audioio_wavefile_obj_t* self, bool single_channel,
                                           bool* single_buffer, bool* samples_signed,
                                           uint32_t* max_buffer_length, uint8_t* spacing) {
    *single_buffer = false;
    *samples_signed = self->bits_per_sample > 8;
    *max_buffer_length = 512;
    if (single_channel) {
        *spacing = self->channel_count;
    } else {
        *spacing = 1;
    }
}
