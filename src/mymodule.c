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

STATIC mp_obj_t mymodule_deep_sleep(void) {
  /*
  // store the original value for global interrupt enabling
  uint32_t original_PRIMASK_value;
  original_PRIMASK_value = __get_PRIMASK();
  // enable interrupts by setting PRIMASK to 0
  __set_PRIMASK(0);
  // Tell the NVIC what pin we want to get an interrupt on.
  // Looking at circuitpython/circuitpython/ports/atmel-samd/boards/itsybitsy_m0_express/pins.c
  // I want to use D12.  This is pin_PA19 in the mapping table.
  NVIC_EnableIRQ(PIN_PA19);
  // TBD: Interrupt priority level????
  __set_PRIMASK(original_PRIMASK_value);
  */
  // Now ... EIC...rev your engine...
  turn_on_external_interrupt_controller();

  // Set the PMUX pin funciton to "EIC"
  gpio_set_pin_function(PIN_PA19, GPIO_PIN_FUNCTION_A);

  // Enable the EIC IRQ on the NVIC
  turn_on_cpu_interrupt(pin_PA19.extint_channel);

  // Configure the EIC to trigger an interrupt on HIGH. I hand-rolled this vs using
  // turn_on_eic_channel() since we're setting an extra config (WAKEUP) and are not
  // interested in setting the channel_handler (EIC_Handler).
  uint8_t config_index = pin_PA19.extint_channel / 8;
  uint8_t position = (pin_PA19.extint_channel % 8) * 4;
  uint32_t sense_setting = EIC_CONFIG_SENSE0_HIGH;
  // The following instructions will clear any existing register bits.
  // Use a mask if you don't want to clear them.
  // See the SAMD21 datasheet on Sleep Mode Operation (paragraph 21.6.8, page 310);
  // recommends to use both WAKEUPEN and INTENSET.
  EIC->CONFIG[config_index].reg = sense_setting << position;
  EIC->WAKEUP.reg = pin_PA19.extint_channel << EIC_WAKEUP_WAKEUPEN_Pos;
  EIC->INTENSET.reg = pin_PA19.extint_channel << EIC_INTENSET_EXTINT_Pos;

  // Go to sleep.
  SET_BIT(SCB->SCR,2);
  __WFI();

  return mp_const_none;
}

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
