/**************************************************************************/
/*!
    @file     tusb_config.h
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

#ifndef _TUSB_CONFIG_H_
#define _TUSB_CONFIG_H_

#ifdef __cplusplus
 extern "C" {
#endif

//--------------------------------------------------------------------
// COMMON CONFIGURATION
//--------------------------------------------------------------------
#define CFG_TUSB_MCU                OPT_MCU_NRF5X
#define CFG_TUSB_RHPORT0_MODE       OPT_MODE_DEVICE

#define CFG_TUSB_DEBUG              2

/*------------- RTOS -------------*/
#define CFG_TUSB_OS                 OPT_OS_NONE // be passed from IDE/command line for easy project switching
//#define CFG_TUD_TASK_PRIO         0
//#define CFG_TUD_TASK_QUEUE_SZ     16
//#define CFG_TUD_TASK_STACK_SZ     150

//--------------------------------------------------------------------
// DEVICE CONFIGURATION
//--------------------------------------------------------------------

#define CFG_TUD_ENDOINT0_SIZE       64

/*------------- Descriptors -------------*/

/* Enable auto generated descriptor, tinyusb will try its best to create
 * descriptor ( device, configuration, hid ) that matches enabled CFG_* in this file
 *
 * Note: All CFG_TUD_DESC_* are relevant only if CFG_TUD_DESC_AUTO is enabled
 */
#define CFG_TUD_DESC_AUTO           1

/* USB VID/PID if not defined, tinyusb to use default value
 * Note: different class combination e.g CDC and (CDC + MSC) should have different
 * PID since Host OS will "remembered" device driver after the first plug */
// #define CFG_TUD_DESC_VID          0xCAFE
// #define CFG_TUD_DESC_PID          0x0001

//------------- CLASS -------------//
#define CFG_TUD_CDC                 1
#define CFG_TUD_MSC                 1

#define CFG_TUD_HID                 1
#define CFG_TUD_HID_KEYBOARD        1
#define CFG_TUD_HID_MOUSE           1

/* Use Boot Protocol for Keyboard, Mouse. Enable this will create separated HID interface
 * require more IN endpoints. If disabled, they they are all packed into a single
 * multiple report interface called "Generic". */
#define CFG_TUD_HID_KEYBOARD_BOOT   1
#define CFG_TUD_HID_MOUSE_BOOT      1


//--------------------------------------------------------------------
// CDC
//--------------------------------------------------------------------

// FIFO size of CDC TX and RX
#define CFG_TUD_CDC_RX_BUFSIZE      64
#define CFG_TUD_CDC_TX_BUFSIZE      64

// TX is sent automatically every Start of Frame event.
// If not enabled, application must call tud_cdc_write_flush() periodically
#define CFG_TUD_CDC_FLUSH_ON_SOF    0

//--------------------------------------------------------------------
// MSC
//--------------------------------------------------------------------

// Number of supported Logical Unit Number (At least 1)
#define CFG_TUD_MSC_MAXLUN          1

// Buffer size of Device Mass storage
#define CFG_TUD_MSC_BUFSIZE         512

// Number of Blocks
#define CFG_TUD_MSC_BLOCK_NUM       16

// Block size
#define CFG_TUD_MSC_BLOCK_SZ        512

// Vendor name included in Inquiry response, max 8 bytes
#define CFG_TUD_MSC_VENDOR          "tinyusb"

// Product name included in Inquiry response, max 16 bytes
#define CFG_TUD_MSC_PRODUCT         "tusb msc"

// Product revision string included in Inquiry response, max 4 bytes
#define CFG_TUD_MSC_PRODUCT_REV     "1.0"

//--------------------------------------------------------------------
// HID
//--------------------------------------------------------------------

/* Use the HID_ASCII_TO_KEYCODE lookup if CFG_TUD_HID_KEYBOARD is enabled.
 * This will occupies 256 bytes of ROM. It will also enable the use of 2 extra APIs
 * - tud_hid_keyboard_send_char()
 * - tud_hid_keyboard_send_string()
 */
#define CFG_TUD_HID_ASCII_TO_KEYCODE_LOOKUP 1

//--------------------------------------------------------------------
// USB RAM PLACEMENT
//--------------------------------------------------------------------
#define CFG_TUSB_ATTR_USBRAM
#define CFG_TUSB_MEM_ALIGN          ATTR_ALIGNED(4)


#ifdef __cplusplus
 }
#endif

#endif /* _TUSB_CONFIG_H_ */
