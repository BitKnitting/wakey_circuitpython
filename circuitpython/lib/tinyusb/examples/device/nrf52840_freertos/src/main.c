/**************************************************************************/
/*!
    @file     main.c
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

//--------------------------------------------------------------------+
// INCLUDE
//--------------------------------------------------------------------+
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"

#include "bsp/board.h"
#include "tusb.h"

//--------------------------------------------------------------------+
// MACRO CONSTANT TYPEDEF
//--------------------------------------------------------------------+

//--------------------------------------------------------------------+
// INTERNAL OBJECT & FUNCTION DECLARATION
//--------------------------------------------------------------------+
void print_greeting(void);
void led_blinky_cb(TimerHandle_t xTimer);
void cdc_task(void* params);
void usb_hid_task(void* params);


/*------------- MAIN -------------*/
int main(void)
{
  board_init();
  print_greeting();

  // soft timer for blinky
  TimerHandle_t tm_hdl = xTimerCreate(NULL, pdMS_TO_TICKS(1000), true, NULL, led_blinky_cb);
  xTimerStart(tm_hdl, 0);

  tusb_init();

  // Create task
  xTaskCreate( cdc_task, "cdc", 256, NULL, 2, NULL);

  vTaskStartScheduler();

  NVIC_SystemReset();

  return 0;
}

//--------------------------------------------------------------------+
// USB CDC
//--------------------------------------------------------------------+
void cdc_task(void* params)
{
  (void) params;

  while ( 1 )
  {
    // connected and there are data available
    if ( tud_mounted() && tud_cdc_available() )
    {
      uint8_t buf[64];

      // read and echo back
      uint32_t count = tud_cdc_read(buf, sizeof(buf));

      tud_cdc_write(buf, count);
      tud_cdc_write_flush();
    }

    taskYIELD();
  }
}

//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+
void usb_hid_task(void* params)
{
  (void) params;

  // Poll every 10ms
  static tu_timeout_t tm = { .start = 0, .interval = 10 };

  if ( !tu_timeout_expired(&tm) ) return; // not enough time
  tu_timeout_reset(&tm);

  uint32_t const btn = board_buttons();

  /*------------- Keyboard -------------*/
  if ( tud_hid_keyboard_ready() )
  {
    if ( btn )
    {
      uint8_t keycode[6] = { 0 };

      for(uint8_t i=0; i < 6; i++)
      {
        if ( btn & (1 << i) ) keycode[i] = HID_KEY_A + i;
      }

      tud_hid_keyboard_keycode(0, keycode);
    }else
    {
      // Null means all zeroes keycodes
      tud_hid_keyboard_keycode(0, NULL);
    }
  }


  /*------------- Mouse -------------*/
  if ( tud_hid_mouse_ready() )
  {
    enum { DELTA  = 5 };

    if ( btn & 0x01 ) tud_hid_mouse_move(-DELTA,      0); // left
    if ( btn & 0x02 ) tud_hid_mouse_move( DELTA,      0); // right
    if ( btn & 0x04 ) tud_hid_mouse_move(  0   , -DELTA); // up
    if ( btn & 0x08 ) tud_hid_mouse_move(  0   ,  DELTA); // down
  }
}

uint16_t tud_hid_generic_get_report_cb(uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
  // TODO not Implemented
  return 0;
}

void tud_hid_generic_set_report_cb(uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
  // TODO not Implemented
}

//--------------------------------------------------------------------+
// tinyusb callbacks
//--------------------------------------------------------------------+
void tud_mount_cb(void)
{

}

void tud_umount_cb(void)
{
}

void tud_cdc_rx_cb(uint8_t itf)
{
}

//--------------------------------------------------------------------+
// BLINKING TASK
//--------------------------------------------------------------------+
void led_blinky_cb(TimerHandle_t xTimer)
{
  (void) xTimer;
  static bool led_state = false;

  board_led_control(BOARD_LED0, led_state);
  led_state = 1 - led_state; // toggle
}

//--------------------------------------------------------------------+
// HELPER FUNCTION
//--------------------------------------------------------------------+
void print_greeting(void)
{
  char const * const rtos_name[] =
  {
      [OPT_OS_NONE]      = "None",
      [OPT_OS_FREERTOS]  = "FreeRTOS",
  };

  printf("\n--------------------------------------------------------------------\n");
  printf("-                     Device Demo (a tinyusb example)\n");
  printf("- if you find any bugs or get any questions, feel free to file an\n");
  printf("- issue at https://github.com/hathach/tinyusb\n");
  printf("--------------------------------------------------------------------\n\n");

  printf("This DEVICE demo is configured to support:");
  printf("  - RTOS = %s\n", rtos_name[CFG_TUSB_OS]);
  if (CFG_TUD_CDC          ) puts("  - Communication Device Class");
  if (CFG_TUD_MSC          ) puts("  - Mass Storage");
  if (CFG_TUD_HID_KEYBOARD ) puts("  - HID Keyboard");
  if (CFG_TUD_HID_MOUSE    ) puts("  - HID Mouse");
}
