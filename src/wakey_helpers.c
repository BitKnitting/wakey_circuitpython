//
// hal_gpio.h has the nifty functions for setting GPIO pins (e.g.: toggle, set, get..)
// asf4/samd21/hal/include/hal_gpio.h
#include "hal/include/hal_gpio.h"
#include "samd/external_interrupts.h"
#include "hal/include/hal_delay.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "atmel_start_pins.h"
#include "wakey_helpers.h"
//

// Give a name to the LED pin on the itsy bitsy.
// The CP pin mappings to SamD pin mappings are defined in
// ports/atmel-samd/itsybitsy_m0_express/pins.c
// D13 is for the red LED:
// { MP_ROM_QSTR(MP_QSTR_D13), MP_ROM_PTR(&pin_PA17) },
// which...ultimately...devolves into ... pin 17?
#define LED0 GPIO(GPIO_PORTA, 17)
#pragma GCC push_options
#pragma GCC optimize ("O0")
/**************************************************************************
* void config_GPIO(uint8_t pin)
* Uses the hal driver's gpio API to configure the pin assigned
* to the interrupt.  We got these steps from our interrupt sample app
* created in Atmel Start
* INPUT: The pin number
*/
void config_GPIO(uint8_t pin_number)
{

  gpio_set_pin_direction(pin_number, GPIO_DIRECTION_IN);
  gpio_set_pin_pull_mode(pin_number,
                       // <y> Pull configuration
                       // <id> pad_pull_config
                       // <GPIO_PULL_OFF"> Off
                       // <GPIO_PULL_UP"> Pull-up
                       // <GPIO_PULL_DOWN"> Pull-down
                       GPIO_PULL_DOWN);

  gpio_set_pin_function(pin_number, GPIO_PIN_FUNCTION_A);
}
/**********************************************************************
* void config_EIC(eic_channel)
* All the goop needed to set up an interrupt to wake up the SamD21 from
* standby.  I wrote this stuff up: https://github.com/BitKnitting/wakey_circuitpython/wiki/Wake-up-SamD21-Through-an-Interrupthttps://github.com/BitKnitting/wakey_circuitpython/wiki/Wake-up-SamD21-Through-an-Interrupt
*/
void config_EIC(uint8_t eic_channel)
{
  /* Configure the EIC so that the SamD21 will wake up from standby
  * when the external interrupt assigned to PA19 is triggered.
  */
  uint32_t extint_mask = 1 << eic_channel;
  EIC->WAKEUP.reg |= extint_mask;
  uint8_t config_index = eic_channel / 8;
  uint8_t position = (eic_channel % 8) * 4;
   // First clear the CONFIG register (in this case CONFIG0).
  EIC->CONFIG[config_index].reg &=~ (EIC_CONFIG_SENSE0_Msk << position);
  // Then set to rising with filtering.
  uint32_t sense_setting = EIC_CONFIG_SENSE0_RISE_Val | EIC_CONFIG_FILTEN0;
  EIC->CONFIG[config_index].reg |= sense_setting << position;
  EIC->INTENSET.reg |= extint_mask;
  

  //hri_eic_set_INTEN_reg(EIC, extint_mask);

  NVIC_DisableIRQ(EIC_IRQn);
  NVIC_ClearPendingIRQ(EIC_IRQn);
  NVIC_EnableIRQ(EIC_IRQn);

	hri_eic_set_CTRL_ENABLE_bit(EIC);
}

// Safe and restful sleep...
void time_to_sleep(void){
  __DSB(); // Complete any pending buffer writes.
  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
  __WFI();
}
// Callback
void wakey_interrupt_handler(uint8_t channel) {

  volatile uint32_t flags = hri_eic_read_INTFLAG_reg(EIC) & hri_eic_read_INTEN_reg(EIC);
  hri_eic_clear_INTFLAG_reg(EIC, flags); 
}
#pragma GCC pop_options
