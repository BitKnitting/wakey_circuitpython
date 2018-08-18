//
// hal_gpio.h has the nifty functions for setting GPIO pins (e.g.: toggle, set, get..)
// asf4/samd21/hal/include/hal_gpio.h
#include "hal/include/hal_gpio.h"
#include "samd/external_interrupts.h"
#include "hal/include/hal_delay.h"
#include "atmel_start_pins.h"
#include "lowpower_helpers.h"
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
 * void init_LED(void)
 * Get the red LED on the itsybitsy ready to blink. We use this to indicate
 * if the interrupt woke up the SamD21
 **************************************************************************/
void init_LED(void)
{
  // Set pin direction to output
  gpio_set_pin_direction(LED0, GPIO_DIRECTION_OUT);
  gpio_set_pin_level(LED0,
                   // <y> Initial level
                   // <id> pad_initial_level
                   // <false"> Low
                   // <true"> High
                   false);
  gpio_set_pin_function(LED0, GPIO_PIN_FUNCTION_OFF);
  // Turn LED off
  gpio_set_pin_level(LED0,false);
}
/**************************************************************************
* void config_GPIO(uint8_t pin)
* Uses the hal driver's gpio API to configure the pin assigned
* to the interrupt.  We got these steps from our interrupt sample app
* created in Atmel Start
* INPUT: The SamD21 pin (e.g.: PA02...)
*/
void config_GPIO(uint8_t pin)
{

  gpio_set_pin_direction(pin, GPIO_DIRECTION_IN);
  gpio_set_pin_pull_mode(pin,
                       // <y> Pull configuration
                       // <id> pad_pull_config
                       // <GPIO_PULL_OFF"> Off
                       // <GPIO_PULL_UP"> Pull-up
                       // <GPIO_PULL_DOWN"> Pull-down
                       GPIO_PULL_UP);

  gpio_set_pin_function(pin, GPIO_PIN_FUNCTION_A);

}
/**********************************************************************
* void config_EIC(eic_channel)
* All the goop needed to set up an interrupt to wake up the SamD21 from
* standby.  I wrote this stuff up: https://github.com/BitKnitting/wakey_circuitpython/wiki/Wake-up-SamD21-Through-an-Interrupthttps://github.com/BitKnitting/wakey_circuitpython/wiki/Wake-up-SamD21-Through-an-Interrupt
*/
void config_EIC(uint8_t eic_channel)
{
  // Set up the CTRL register - we'll be using the EIC and want to
  // respond to a software reset.  The hri APIs are included within CP.
  hri_eic_wait_for_sync(EIC);
  if (hri_eic_get_CTRL_reg(EIC, EIC_CTRL_ENABLE)) {
    hri_eic_write_CTRL_reg(EIC, 0);
    hri_eic_wait_for_sync(EIC);
  }
  hri_eic_write_CTRL_reg(EIC, EIC_CTRL_SWRST);
  hri_eic_wait_for_sync(EIC);
  /*
  *  Write a 1 in the EIC’s WAKEUP register in the bit for this pin's external channel.
  *  i.e.: Tell the EIC to wake up the SamD21 when the interrupt triggers and the
  *  SamD21 is in sleep mode.
  */
  uint32_t extint_mask = 1 << eic_channel;
  EIC->WAKEUP.reg |= extint_mask;
  /* Tell the EIC to trigger the interrupt when the level changes to HIGH.
  * This is done by figuring out how to talk with the CONFIGn register(s).
  */
  uint8_t config_index = eic_channel / 8;
  uint8_t position = (eic_channel % 8) * 4;
  EIC->CONFIG[config_index].reg &=~ (EIC_CONFIG_SENSE0_Msk << position);
  EIC->CONFIG[config_index].reg |= EIC_CONFIG_SENSE0_LOW_Val << position;
  /*
  * let NVIC know EIC will be communicating with it.
  */
  NVIC_DisableIRQ(EIC_IRQn);
  NVIC_ClearPendingIRQ(EIC_IRQn);
  NVIC_EnableIRQ(EIC_IRQn);
  /*
  * Enable the interrupt
  */
  EIC->INTENSET.reg = EIC_INTENSET_EXTINT(extint_mask);
}
/*
 * external_interrupts.c contains the code to handle callbacks to execute
 * when the interrupt occurs.  The callback gets registered by putting
 * an entry into channel_handler[EIC_EXTINT_NUM];
 * This is done in the turn_on_eic_channel() function.  However, this
 * function also "double duties" to config the interrupt for what will
 * trigger it (edge or level), and to use a filter.  We are not using a
 * filter and we want a different config for the sense.  So the
 * only thing we need to make sure external_interrupts has is the correct.
 * channel_handler[EIC_EXTINT_NUM] entry.
*/
void register_callback(uint8_t eic_channel, uint8_t channel_interrupt_handler) {
  // I added this function to external_interrupts.c
  set_eic_channel_handler(eic_channel,channel_interrupt_handler);
}
// Safe and restful sleep...
void time_to_sleep(void){
  __DSB(); // Complete any pending buffer writes.
  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
  __WFI();
}
// Callback
void wakey_interrupt_handler(uint8_t channel) {
	for (int i = 0;i < 4;i++) {
		delay_ms(200);
		gpio_toggle_pin_level(LED0);
	}
}
#pragma GCC pop_options
