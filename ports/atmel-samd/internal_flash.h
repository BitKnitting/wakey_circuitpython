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
#ifndef MICROPY_INCLUDED_ATMEL_SAMD_INTERNAL_FLASH_H
#define MICROPY_INCLUDED_ATMEL_SAMD_INTERNAL_FLASH_H

#include <stdbool.h>

#include "mpconfigport.h"

#include "sam.h"

#define FLASH_ROOT_POINTERS

#ifdef SAMD51
#define TOTAL_INTERNAL_FLASH_SIZE (FLASH_SIZE / 2)
#endif

#ifdef SAMD21
#define TOTAL_INTERNAL_FLASH_SIZE 0x010000
#endif

#define INTERNAL_FLASH_MEM_SEG1_START_ADDR (FLASH_SIZE - TOTAL_INTERNAL_FLASH_SIZE - CIRCUITPY_INTERNAL_NVM_SIZE)
#define INTERNAL_FLASH_PART1_START_BLOCK (0x1)
#define INTERNAL_FLASH_PART1_NUM_BLOCKS (TOTAL_INTERNAL_FLASH_SIZE / FILESYSTEM_BLOCK_SIZE)

#define INTERNAL_FLASH_SYSTICK_MASK    (0x1ff) // 512ms
#define INTERNAL_FLASH_IDLE_TICK(tick) (((tick) & INTERNAL_FLASH_SYSTICK_MASK) == 2)

void internal_flash_init(void);
uint32_t internal_flash_get_block_size(void);
uint32_t internal_flash_get_block_count(void);
void internal_flash_irq_handler(void);
void internal_flash_flush(void);
bool internal_flash_read_block(uint8_t *dest, uint32_t block);
bool internal_flash_write_block(const uint8_t *src, uint32_t block);

// these return 0 on success, non-zero on error
mp_uint_t internal_flash_read_blocks(uint8_t *dest, uint32_t block_num, uint32_t num_blocks);
mp_uint_t internal_flash_write_blocks(const uint8_t *src, uint32_t block_num, uint32_t num_blocks);

extern const struct _mp_obj_type_t internal_flash_type;

struct _fs_user_mount_t;
void flash_init_vfs(struct _fs_user_mount_t *vfs);

#endif  // MICROPY_INCLUDED_ATMEL_SAMD_INTERNAL_FLASH_H
