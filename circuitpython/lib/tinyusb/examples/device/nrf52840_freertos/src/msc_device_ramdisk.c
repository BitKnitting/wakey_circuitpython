/**************************************************************************/
/*!
    @file     msc_device_ramdisk.c
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
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    This file is part of the tinyusb stack.
*/
/**************************************************************************/

#include "msc_device_app.h"

#if CFG_TUD_MSC && defined (MSCD_APP_RAMDISK)

//--------------------------------------------------------------------+
// MACRO CONSTANT TYPEDEF
//--------------------------------------------------------------------+

//--------------------------------------------------------------------+
// INTERNAL OBJECT & FUNCTION DECLARATION
//--------------------------------------------------------------------+
CFG_TUSB_ATTR_USBRAM CFG_TUSB_MEM_ALIGN
uint8_t msc_device_ramdisk[DISK_BLOCK_NUM][DISK_BLOCK_SIZE] =
{
  //------------- Boot Sector -------------//
  // byte_per_sector    = DISK_BLOCK_SIZE; fat12_sector_num_16  = DISK_BLOCK_NUM;
  // sector_per_cluster = 1; reserved_sectors = 1;
  // fat_num            = 1; fat12_root_entry_num = 16;
  // sector_per_fat     = 1; sector_per_track = 1; head_num = 1; hidden_sectors = 0;
  // drive_number       = 0x80; media_type = 0xf8; extended_boot_signature = 0x29;
  // filesystem_type    = "FAT12   "; volume_serial_number = 0x1234; volume_label = "tinyusb msc";
  [0] =
  {
      0xEB, 0x3C, 0x90, 0x4D, 0x53, 0x44, 0x4F, 0x53, 0x35, 0x2E, 0x30, 0x00, 0x02, 0x01, 0x01, 0x00,
      0x01, 0x10, 0x00, 0x10, 0x00, 0xF8, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x29, 0x34, 0x12, 0x00, 0x00, 0x74, 0x69, 0x6E, 0x79, 0x75,
      0x73, 0x62, 0x20, 0x6D, 0x73, 0x63, 0x46, 0x41, 0x54, 0x31, 0x32, 0x20, 0x20, 0x20, 0x00, 0x00,
      [510] = 0x55, [511] = 0xAA // FAT magic code
  },

  //------------- FAT12 Table -------------//
  [1] =
  {
      0xF8, 0xFF, 0xFF, 0xFF, 0x0F // // first 2 entries must be F8FF, third entry is cluster end of readme file
  },

  //------------- Root Directory -------------//
  [2] =
  {
      // first entry is volume label
      0x54, 0x49, 0x4E, 0x59, 0x55, 0x53, 0x42, 0x20, 0x4D, 0x53, 0x43, 0x08, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4F, 0x6D, 0x65, 0x43, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      // second entry is readme file
      'R' , 'E' , 'A' , 'D' , 'M' , 'E' , ' ' , ' ' , 'T' , 'X' , 'T' , 0x20, 0x00, 0xC6, 0x52, 0x6D,
      0x65, 0x43, 0x65, 0x43, 0x00, 0x00, 0x88, 0x6D, 0x65, 0x43, 0x02, 0x00,
      sizeof(README_CONTENTS)-1, 0x00, 0x00, 0x00 // readme's filesize (4 Bytes)
  },

  //------------- Readme Content -------------//
  [3] = README_CONTENTS
};


// Callback invoked when received READ10 command.
// Copy disk's data to buffer (up to bufsize) and return number of copied bytes.
int32_t tud_msc_read10_cb(uint8_t lun, uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize)
{
  uint8_t* addr = msc_device_ramdisk[lba] + offset;
  memcpy(buffer, addr, bufsize);

  return bufsize;
}

// Callback invoked when received WRITE10 command.
// Process data in buffer to disk's storage and return number of written bytes
int32_t tud_msc_write10_cb(uint8_t lun, uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize)
{
  uint8_t* addr = msc_device_ramdisk[lba] + offset;
  memcpy(addr, buffer, bufsize);

  return bufsize;
}

//--------------------------------------------------------------------+
// HELPER
//--------------------------------------------------------------------+

#endif
