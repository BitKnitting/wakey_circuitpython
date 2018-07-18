#include "py/obj.h"
#include "py/runtime.h"
#include "samd/external_interrupts.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "samd/pins.h"
#include "atmel_start_pins.h"
#include "hal/include/hal_gpio.h"
#include "hpl/gclk/hpl_gclk_base.h"

#pragma GCC push_options
#pragma GCC optimize ("O0")

#define SET_BIT(REG, BIT)     ((REG) |= (BIT))
#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))
#define READ_BIT(REG, BIT)    ((REG) & (BIT))

STATIC uint32_t extint_mask;

STATIC void go_to_sleep(void);

STATIC uint32_t extint_mask;
// STATIC void wakeup_interrupt_handler(uint8_t channel);
//
// STATIC void wakeup_interrupt_handler(uint8_t channel) {
//   int i = 4;
//   i += 1;
//
// }
/*************************************************************************
*/
STATIC void go_to_sleep() {
  SET_BIT(SCB->SCR,2);
  __WFI();
}
/*************************************************************************
*/
STATIC mp_obj_t debug_test_hello(void) {
    int i = 5;
    i += 1;
  /*
   Testing with D12 = PA19 = EXTINT 3
  */
  // Tell NVIC we'll be using an external interrupt.
  // We also gave it a priority of 0. We did this
  // because "most" examples did this (e.g.: rtc_zero)
  NVIC_DisableIRQ(EIC_IRQn);
  NVIC_ClearPendingIRQ(EIC_IRQn);
  NVIC_SetPriority(EIC_IRQn, 0);
  NVIC_EnableIRQ(EIC_IRQn);
  // Enable EIC
  //PM->APBAMASK.bit.EIC_ = true;
  // TBD: Need clock.  I do not believe a clock is needed
  // trigger is HIGH or LOW
  //_gclk_enable_channel(EIC_GCLK_ID, GCLK_CLKCTRL_GEN_GCLK0_Val);
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
  gpio_set_pin_function(pin_PA19.pin, GPIO_PIN_FUNCTION_A);
  uint8_t config_index = extint / 8;
  uint8_t position = (extint % 8) * 4;
  // Configure the interrupt mode
  // Reset sense mode, important when changing trigger mode during runtime
  EIC->CONFIG[config_index].reg &=~ (EIC_CONFIG_SENSE0_Msk << position);
  // Only LOW and HIGH avaible when no clock configed for EIC and in standby mode.
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
   int i = 5;
   i += 1;
     // Clear the interrupt
   EIC->INTFLAG.reg = extint_mask;
 }
/*********************************************************************************************/

STATIC MP_DEFINE_CONST_FUN_OBJ_0(debug_test_hello_obj, debug_test_hello);


STATIC const mp_map_elem_t debug_test_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_debug_test) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_hello), (mp_obj_t)&debug_test_hello_obj },
};

STATIC MP_DEFINE_CONST_DICT (
    mp_module_debug_test_globals,
    debug_test_globals_table
);

const mp_obj_module_t mp_module_debug_test = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_debug_test_globals,
};
#pragma GCC pop_options
