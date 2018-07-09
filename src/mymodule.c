#include "py/obj.h"
#include "py/runtime.h"
#include "samd/external_interrupts.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "samd/pins.h"
#include "atmel_start_pins.h"
#include "hal/include/hal_gpio.h"



#define SET_BIT(REG, BIT)     ((REG) |= (BIT))
#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))
#define READ_BIT(REG, BIT)    ((REG) & (BIT))

STATIC uint32_t extint_mask;

void go_to_sleep(void);

/*************************************************************************
*/
void go_to_sleep() {
  SET_BIT(SCB->SCR,2);
  __WFI();
}
/*************************************************************************
*/
STATIC mp_obj_t mymodule_deep_sleep(void) {
  /*
   Testing with D12 = PA19 = EXTINT 3
  */

  NVIC_DisableIRQ(EIC_IRQn);
  NVIC_ClearPendingIRQ(EIC_IRQn);
  NVIC_SetPriority(EIC_IRQn, 0);
  NVIC_EnableIRQ(EIC_IRQn);
  // Enable EIC
  EIC->CTRL.bit.ENABLE = 1;
  while (EIC->STATUS.bit.SYNCBUSY != 0) {}
  // Enable wakeup capability on pin in case being used during sleep
  // HARD coded to D12/PA19/extint 3 (geez - a pin is a pin is a pin....)
  // See: https://github.com/arduino/ArduinoCore-samd/blob/master/variants/arduino_zero/variant.cpp
  // for pin mappings between Arduino (circuit python) / m0 pin / extint #
  //  * | pin #: 12  | digital high: ~12  |  m0 pin: PA19  | EIC/EXTINT[3]
  const uint8_t extint = 3;
  extint_mask = 1 << extint;
  EIC->WAKEUP.reg |= extint_mask;
  // Set the PMUX pin function to "EIC"
  // I guess this is right.  Don't really know.
  // ooohhh...funky!!!
  gpio_set_pin_function(pin_PA19.pin, GPIO_PIN_FUNCTION_A);
  uint8_t config_index = extint / 8;
  uint8_t position = (extint % 8) * 4;
  // Only LOW and HIGH avaible when no clock configed for EIC and in standby mode.
  // so testing with LOW.
  EIC->CONFIG[config_index].reg |= EIC_CONFIG_SENSE0_LOW_Val << position;

  // Enable the interrupt
  EIC->INTENSET.reg = EIC_INTENSET_EXTINT(extint_mask);

  go_to_sleep();

  return mp_const_none;
}
/*
 * External Interrupt Controller - external_interrupts.c has this.
 */
 void EIC_Handler(void)
 {

     // Clear the interrupt
   EIC->INTFLAG.reg = extint_mask;
 }
/*********************************************************************************************/

STATIC MP_DEFINE_CONST_FUN_OBJ_0(mymodule_deep_sleep_obj, mymodule_deep_sleep);


STATIC const mp_map_elem_t mymodule_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_mymodule) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_deep_sleep), (mp_obj_t)&mymodule_deep_sleep_obj },
};

STATIC MP_DEFINE_CONST_DICT (
    mp_module_mymodule_globals,
    mymodule_globals_table
);

const mp_obj_module_t mp_module_mymodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_mymodule_globals,
};
