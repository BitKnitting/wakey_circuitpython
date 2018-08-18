#include "py/obj.h"
#include "py/runtime.h"
// for gpio and pin functions

#include "atmel_start_pins.h"
#include "hal/include/hal_gpio.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "samd/pins.h"


// for new_status_color
#include "supervisor/shared/rgb_led_status.h"

//#pragma GCC push_options
//#pragma GCC optimize ("O0")

STATIC void go_to_sleep(void);

STATIC void go_to_sleep() {

  __DSB(); // Complete any pending buffer writes.
  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
  __WFI();
}
// Function on the module
// sleep takes a pin to use as the external interrupt
// to wake up from m0 standby mode.
STATIC mp_obj_t  sleep(mp_obj_t pin_in) {
  // What other things should we do to lower current draw by peripherals?
  new_status_color(BLACK);
  // Get the pin that was passed in (e.g.: D12)
  assert_pin(pin_in,false);
  mcu_pin_obj_t *pin = MP_OBJ_TO_PTR(pin_in);
  assert_pin_free(pin);
  digitalio_digitalinout_obj_t int_pin;
  common_hal_digitalio_digitalinout_construct(&int_pin, pin);
  // Tell NVIC we'll be using an external interrupt.
  // We also gave it a priority of 0. We did this
  // because "most" examples did this (e.g.: rtc_zero)
  NVIC_DisableIRQ(EIC_IRQn);
  NVIC_ClearPendingIRQ(EIC_IRQn);
  NVIC_SetPriority(EIC_IRQn, 0);
  NVIC_EnableIRQ(EIC_IRQn);
  // Enable EIC
  EIC->CTRL.bit.ENABLE = 1;
  while (EIC->STATUS.bit.SYNCBUSY != 0) {}
  // Enable wakeup capability on pin
  uint8_t extint_channel = int_pin.pin->extint_channel;
  uint32_t extint_mask = 1 << extint_channel;
  EIC->WAKEUP.reg |= extint_mask;
  // Set the pin to LOW
  gpio_set_pin_pull_mode(int_pin.pin->pin,GPIO_PULL_DOWN);
  // Set the PMUX pin function to "EIC"
  gpio_set_pin_function(int_pin.pin->pin, GPIO_PIN_FUNCTION_A);
  // Configure the pin to trigger when pin goes from LOW to HIGH
  uint8_t config_index = extint_channel / 8;
  uint8_t position = (extint_channel % 8) * 4;
  // Reset sense mode
  EIC->CONFIG[config_index].reg &=~ (EIC_CONFIG_SENSE0_Msk << position);
  // Configure to trigger when 3.3V applied to pin.
  // Testing led me to use EIC_CONFIG_SENSE0_LOW_VAL.
  EIC->CONFIG[config_index].reg |= EIC_CONFIG_SENSE0_LOW_Val << position;
  // Enable the interrupt
  EIC->INTENSET.reg = EIC_INTENSET_EXTINT(extint_mask);

  // Safe and restful sleep...
  go_to_sleep();
  return mp_const_none;
}
// The Sleep function takes in the (CircuitPython) pin
// that will be configured to wake up the mo from standby
// mode when...
STATIC MP_DEFINE_CONST_FUN_OBJ_1(sleep_obj, sleep);

/*
* lowpower module
*/
// Dictionary of globals
STATIC const mp_map_elem_t lowpower_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_lowpower) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_sleep), (mp_obj_t)&sleep_obj },
};
STATIC MP_DEFINE_CONST_DICT(lowpower_module_globals, lowpower_module_globals_table);
// Define the module
const mp_obj_module_t lowpower_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&lowpower_module_globals,
};
//#pragma GCC pop_options
