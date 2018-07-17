#include "py/obj.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "samd/external_interrupts.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "samd/pins.h"
#include "atmel_start_pins.h"
#include "hal/include/hal_gpio.h"

#pragma GCC push_options
#pragma GCC optimize ("O0")
STATIC uint32_t extint_mask;
void wakeup_interrupt_handler(uint8_t channel);

void wakeup_interrupt_handler(uint8_t channel) {
  int i = 4;
  i += 1;
}

STATIC mp_obj_t debug_test_hello(void) {
    int i = 5;
    i += 1;

    // Set the wakeup bit.
    extint_mask = 1 << pin_PA19.extint_channel;
    EIC->WAKEUP.reg |= extint_mask;
    // Check to see if the EIC is enabled and start it up if its not.'
    if (eic_get_enable() == 0) {
        eic_set_enable(true);
    }

    // See Table 7-1 (Port Function Mapping) in the datashett.
    gpio_set_pin_function(pin_PA19.pin, GPIO_PIN_FUNCTION_A);

    NVIC_SetPriority(EIC_IRQn, 0);
    //This function does alot...
    //turn_on_eic_channel(pin_PA19.extint_channel, EIC_CONFIG_SENSE0_HIGH_Val, EIC_HANDLER_WAKEUP);
    turn_on_eic_channel(pin_PA19.extint_channel, EIC_CONFIG_SENSE0_HIGH_Val, EIC_HANDLER_NO_INTERRUPT);

    return mp_const_none;
}
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
