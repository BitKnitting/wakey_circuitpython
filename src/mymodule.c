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

void configClock(void);
void configEIC(void);
void configNVIC(void);
void go_to_sleep(void);
/*************************************************************************
*/
void configClock() {

// 14.8.5 Generic Clock Generator Division -
GCLK->GENDIV.reg = GCLK_GENDIV_DIV (1);
while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY)
  ;

// 14.8.4 Generic Clock Generator Control
GCLK->GENCTRL.reg = GCLK_GENCTRL_ID (1) | GCLK_GENCTRL_SRC (GCLK_SOURCE_OSCULP32K) | GCLK_GENCTRL_RUNSTDBY | GCLK_GENCTRL_GENEN;
while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY)
  ;
  // 14.8.3 Generic Clock Control register (p. 106)
GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID_EIC | GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK1;
PM->APBAMASK.bit.EIC_ = true;
}
/*************************************************************************
*/
void configEIC() {
  EIC->INTENSET.reg = pin_PA19.extint_channel << EIC_INTENSET_EXTINT_Pos;
  EIC->WAKEUP.reg = pin_PA19.extint_channel << EIC_WAKEUP_WAKEUPEN_Pos;
  uint8_t config_index = pin_PA19.extint_channel / 8;
  uint8_t position = (pin_PA19.extint_channel % 8) * 4;
  EIC->CONFIG[config_index].reg = EIC_CONFIG_SENSE0_HIGH << position;
}
/*************************************************************************
*/
void configNVIC() {
  turn_on_cpu_interrupt(pin_PA19.extint_channel);
  NVIC_SetPriority(EIC_IRQn, 0x00);
}
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

  */
  configClock();
  configEIC();
  configNVIC();
  // Set the PMUX pin function to "EIC"
  gpio_set_pin_function(pin_PA19.pin, GPIO_PIN_FUNCTION_A);

  eic_set_enable(true);

  go_to_sleep();

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
