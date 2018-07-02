#include "py/obj.h"
#include "py/runtime.h"
#include "samd/external_interrupts.h"


#define SET_BIT(REG, BIT)     ((REG) |= (BIT))
#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))
#define READ_BIT(REG, BIT)    ((REG) & (BIT))

STATIC mp_obj_t mymodule_deep_sleep(void) {
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
  // Now ... EIC...rev your engine...
  turn_on_external_interrupt_controller();
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
