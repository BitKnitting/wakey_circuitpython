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
void config32kOSC(void);

/* Attach peripheral clock to 32k oscillator */
void configClock() {
  GCLK->GENDIV.reg = GCLK_GENDIV_ID(2)|GCLK_GENDIV_DIV(4);
  while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY)
    ;
  GCLK->GENCTRL.reg = (GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_XOSC32K | GCLK_GENCTRL_ID(2) | GCLK_GENCTRL_DIVSEL );
  while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY)
    ;
  //NOTE: Changed RTC_GCLK_ID to EIC_GCLK_ID ....
  GCLK->CLKCTRL.reg = (uint32_t)((GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK2 | (EIC_GCLK_ID << GCLK_CLKCTRL_ID_Pos)));
  while (GCLK->STATUS.bit.SYNCBUSY)
    ;
}

/*
 * Private Utility Functions
 */

/* Configure the 32768Hz Oscillator */
void config32kOSC()
{
  SYSCTRL->XOSC32K.reg = SYSCTRL_XOSC32K_ONDEMAND |
                         SYSCTRL_XOSC32K_RUNSTDBY |
                         SYSCTRL_XOSC32K_EN32K |
                         SYSCTRL_XOSC32K_XTALEN |
                         SYSCTRL_XOSC32K_STARTUP(6) |
                         SYSCTRL_XOSC32K_ENABLE;
}
STATIC mp_obj_t mymodule_deep_sleep(void) {
  // This was in RTC_Zero...figured it couldn't hurt?
  PM->APBAMASK.reg |= PM_APBAMASK_EIC; // turn on digital interface clock
  // Pick an oscillator and turn on the RUNSTDBY bit.
  // TBD: copy/pasted RTC_Zero..but what about the low power oscillator?
  config32kOSC();
  // configure the clock for the EIC....
  configClock();
  NVIC_EnableIRQ(PIN_PA19); // enable EIC interrupt
  NVIC_SetPriority(PIN_PA19, 0x00);
  // Start up the EIC.
  turn_on_external_interrupt_controller();
  // If it's good enough for Pulsein.c....
  gpio_set_pin_function(pin_PA19.pin, GPIO_PIN_FUNCTION_A);
  turn_on_cpu_interrupt(pin_PA19.extint_channel);
  // configure EIC to wake up device when CPU is in STANDBY
  // Allow pin PA19 to wake up the CPU from sleep mode.
  EIC->WAKEUP.reg = pin_PA19.extint_channel << EIC_WAKEUP_WAKEUPEN_Pos;
  // INTENSET -> Enable the interrupt (in this case on pin PA19)
  EIC->INTENSET.reg = pin_PA19.extint_channel << EIC_INTENSET_EXTINT_Pos;
  // I copy/pasted this from sommersoft...I don't understand how to get to these
  // values... for EIC->CONFIG....
  // Configure the EIC to trigger an interrupt on HIGH. I hand-rolled this vs using
  // turn_on_eic_channel() since we're setting an extra config (WAKEUP) and are not
  // interested in setting the channel_handler (EIC_Handler).
  uint8_t config_index = pin_PA19.extint_channel / 8;
  uint8_t position = (pin_PA19.extint_channel % 8) * 4;
  uint32_t sense_setting = EIC_CONFIG_SENSE0_HIGH;
  EIC->CONFIG[config_index].reg = sense_setting << position;

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
