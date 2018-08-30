/**************************************************************************/
/*!
    @file     tusb_descriptors.c
    @author   hathach (tinyusb.org)

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2013, hathach (tinyusb.org)
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    3. Neither the name of the copyright holders nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    INCLUDING NEGLIGENCE OR OTHERWISE ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    This file is part of the tinyusb stack.
*/
/**************************************************************************/

#include "tusb.h"

//--------------------------------------------------------------------+
// STRING DESCRIPTORS
//--------------------------------------------------------------------+
#define STRING_LEN_UNICODE(n) (2 + (2*(n))) // also includes 2 byte header
#define ENDIAN_BE16_FROM( high, low) ENDIAN_BE16(high << 8 | low)

// array of pointer to string descriptors
uint16_t const * const string_desc_arr [] =
{
    [0] = (uint16_t []) { // supported language
        ENDIAN_BE16_FROM( STRING_LEN_UNICODE(1), TUSB_DESC_STRING ),
        0x0409 // English
    },

    [1] = (uint16_t []) { // manufacturer
        ENDIAN_BE16_FROM( STRING_LEN_UNICODE(11), TUSB_DESC_STRING),
        't', 'i', 'n', 'y', 'u', 's', 'b', '.', 'o', 'r', 'g' // len = 11
    },

    [2] = (uint16_t []) { // product
        ENDIAN_BE16_FROM( STRING_LEN_UNICODE(14), TUSB_DESC_STRING),
        't', 'i', 'n', 'y', 'u', 's', 'b', ' ', 'd', 'e', 'v', 'i', 'c', 'e' // len = 14
    },

    [3] = (uint16_t []) { // serials
        ENDIAN_BE16_FROM( STRING_LEN_UNICODE(4), TUSB_DESC_STRING),
        '1', '2', '3', '4' // len = 4
    },

    [4] = (uint16_t []) { // CDC Interface
        ENDIAN_BE16_FROM( STRING_LEN_UNICODE(3), TUSB_DESC_STRING),
        'c', 'd', 'c' // len = 3
    },

    [5] = (uint16_t []) { // Keyboard Interface
        ENDIAN_BE16_FROM( STRING_LEN_UNICODE(5), TUSB_DESC_STRING),
        'm', 'o', 'u', 's', 'e' // len = 5
    },

    [6] = (uint16_t []) { // Keyboard Interface
        ENDIAN_BE16_FROM( STRING_LEN_UNICODE(8), TUSB_DESC_STRING),
        'k', 'e', 'y', 'b', 'o', 'a', 'r', 'd' // len = 8
    },

    [7] = (uint16_t []) { // MSC Interface
        ENDIAN_BE16_FROM( STRING_LEN_UNICODE(3), TUSB_DESC_STRING),
        'm', 's', 'c' // len = 3
    }
};


// tud_desc_set is required by tinyusb stack
// since CFG_TUD_DESC_AUTO is enabled, we only need to set string_arr
tud_desc_set_t tud_desc_set =
{
    .device     = NULL,
    .config     = NULL,
    .string_arr = (uint8_t const **) string_desc_arr,
    .hid_report = NULL
};
