#ifndef WAKEY_HELPERS___H
#define WAKEY_HELPERS___H

// The others are defined in peripherals/samd/external_interrupts.h
#define EIC_HANDLER_WAKEY 0x3

// included to resolve uint8_t
//
#include <stdint.h>
/**************************************************************************
 * void init_LED(void)
 * Get the red LED on the itsybitsy ready to blink. We use this to indicate
 * if the interrupt woke up the SamD21
 **************************************************************************/
void init_LED(void);
/**************************************************************************
* void config_GPIO(uint8_t pin)
* Uses the hal driver's gpio API to configure the pin assigned
* to the interrupt.  We got these steps from our interrupt sample app
* created in Atmel Start
* INPUT: The SamD21 pin (e.g.: PA02...)
*/
void config_GPIO(uint8_t pin);
/**********************************************************************
* void config_EIC(eic_channel)
* All the goop needed to set up an interrupt to wake up the SamD21 from
* standby.  I wrote this stuff up: https://github.com/BitKnitting/wakey_circuitpython/wiki/Wake-up-SamD21-Through-an-Interrupthttps://github.com/BitKnitting/wakey_circuitpython/wiki/Wake-up-SamD21-Through-an-Interrupt
*/
void config_EIC(uint8_t eic_channel);
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
void register_callback(uint8_t eic_channel, uint8_t channel_interrupt_handler);
// Safe and restful sleep...
void time_to_sleep(void);
// Callback
void wakey_interrupt_handler(uint8_t channel);
//
// These are defined in circuitpython/ports/atmel-samd/peripherals/samd/external_interrupts.h
//
//#define EIC_HANDLER_NO_INTERRUPT 0x0
//#define EIC_HANDLER_PULSEIN 0x1
//#define EIC_HANDLER_INCREMENTAL_ENCODER 0x2
#define EIC_HANDLER_WAKEY 0x3

#endif
