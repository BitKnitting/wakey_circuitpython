#ifndef WAKEY_HELPERS___H
#define WAKEY_HELPERS___H

// The others are defined in peripherals/samd/external_interrupts.h
#define EIC_HANDLER_WAKEY 0x3
#include "shared-bindings/microcontroller/Pin.h"
// included to resolve uint8_t
//
#include <stdint.h>

void config_EIC(uint8_t pin, uint8_t eic_channel);

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
