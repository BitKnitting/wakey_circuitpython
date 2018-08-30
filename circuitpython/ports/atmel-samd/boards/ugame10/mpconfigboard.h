#define MICROPY_HW_BOARD_NAME "uGame10"
#define MICROPY_HW_MCU_NAME "samd21e18"

// Salae reads 12mhz which is the limit even though we set it to the
// safer 8mhz.
#define SPI_FLASH_BAUDRATE (8000000)

#define SPI_FLASH_MOSI_PIN          PIN_PA16
#define SPI_FLASH_MISO_PIN          PIN_PA19
#define SPI_FLASH_SCK_PIN           PIN_PA17
#define SPI_FLASH_CS_PIN            PIN_PA18
#define SPI_FLASH_MOSI_PIN_FUNCTION PINMUX_PA16D_SERCOM3_PAD0
#define SPI_FLASH_MISO_PIN_FUNCTION PINMUX_PA19D_SERCOM3_PAD3
#define SPI_FLASH_SCK_PIN_FUNCTION  PINMUX_PA17D_SERCOM3_PAD1
#define SPI_FLASH_SERCOM            SERCOM3
#define SPI_FLASH_SERCOM_INDEX      3
#define SPI_FLASH_MOSI_PAD          0
#define SPI_FLASH_MISO_PAD          3
#define SPI_FLASH_SCK_PAD           1
// <o> Transmit Data Pinout
// <0x0=>PAD[0,1]_DO_SCK
// <0x1=>PAD[2,3]_DO_SCK
// <0x2=>PAD[3,1]_DO_SCK
// <0x3=>PAD[0,3]_DO_SCK
#define SPI_FLASH_DOPO              0
#define SPI_FLASH_DIPO              3   // same as MISO pad

// These are pins not to reset.
#define MICROPY_PORT_A        (PORT_PA16 | PORT_PA17 | PORT_PA18 | PORT_PA19 |\
                               PORT_PA24 | PORT_PA25)
#define MICROPY_PORT_B        (0)
#define MICROPY_PORT_C        (0)

#define CALIBRATE_CRYSTALLESS 1

#include "external_flash/external_flash.h"

// If you change this, then make sure to update the linker scripts as well to
// make sure you don't overwrite code.
#define CIRCUITPY_INTERNAL_NVM_SIZE 256

#define BOARD_FLASH_SIZE (0x00040000 - 0x2000 - CIRCUITPY_INTERNAL_NVM_SIZE)

#include "external_flash/devices.h"

#define EXTERNAL_FLASH_DEVICE_COUNT 1
#define EXTERNAL_FLASH_DEVICES S25FL216K

#include "external_flash/external_flash.h"

#define EXTRA_BUILTIN_MODULES \
    { MP_OBJ_NEW_QSTR(MP_QSTR_audioio), (mp_obj_t)&audioio_module }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_gamepad),(mp_obj_t)&gamepad_module }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR__stage), (mp_obj_t)&stage_module }

// USB is always used internally so skip the pin objects for it.
#define IGNORE_PIN_PA24     1
#define IGNORE_PIN_PA25     1
