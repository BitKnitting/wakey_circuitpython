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

#pragma GCC push_options
#pragma GCC optimize ("O0")

uint8_t pin;
void config_EIC(uint8_t p, uint8_t channel)
{
  pin = p;
  // Configure the GPIO
  gpio_set_pin_direction(pin, GPIO_DIRECTION_IN);
  gpio_set_pin_pull_mode(pin,GPIO_PULL_DOWN);
  gpio_set_pin_function(pin, GPIO_PIN_FUNCTION_A);

  // Configure the EIC registers

  // The first one we'll configure is the WAKEUP register.  Let the SamD21 know we want to be woken
  // up when an interrupt happens on this pin.
  uint32_t extint_mask = 1 << channel;
  EIC->WAKEUP.reg |= extint_mask;

  // We'll start using functions CP graciously provides for external interrupts to "simplify" setting
  // EIC registers and the callback.

  // Let's configure how we want the interrupt to be triggered.  We'll set it to trigger when the
  // voltage rises from LOW to HIGH.
  // (see ports/atmel-samd/peripherals/samd/external_interrupts.c)

  turn_on_eic_channel(channel,EIC_CONFIG_SENSE0_RISE_Val,EIC_HANDLER_WAKEY);

  // Note: EIC_HANDLER_WAKEY identifies our callback function to the system-wide EIC interrupt handler,
  // EIC_Handler() (see ports/atmel-samd/peripherals/samd/samd21/external_interrupts.c)
  // We need to let external_interrupt_handler() know about wakey_interrupt_handler(uint8_t channel)
  // (see ..peripherals/samd/external_interrupts.c)
  // ALSO - turn_on_eic_channel() calls turn_on_cpu_interrupt().  This function lets NVIC know to keep
  // an eye out for traffic coming in from the EIC.

  // Enable the Interrupt.
  hri_eic_set_INTEN_reg(EIC, extint_mask);

  // Associate the clock with the EIC and set the CTRL bit to Enable.
  turn_on_external_interrupt_controller();
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
  turn_off_eic_channel(channel);
}
#pragma GCC pop_options
