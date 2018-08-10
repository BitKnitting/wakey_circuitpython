#include "atmel_start.h"
#include "main.h"
#include "ext_interrupt_driver.h"
#include <util/delay.h>

#define LED0_ON LED0_set_level(false);
#define LED0_OFF LED0_set_level(true);
#define LED1_ON LED1_set_level(false);
#define LED1_OFF LED1_set_level(true);

/*! \brief Interrupt handler for External interrupt INT0
 *  Blinks LED0  */
ISR(INT0_vect)
{
	// Turn on LED0
	LED0_ON

	// wait for some delay
	_delay_ms(CONF_DELAY);

	// Turn off LED0
	LED0_OFF
}

/*! \brief Interrupt handler for Pin change interrupt PCINT0
 *  Blinks LED1  */
ISR(PCINT0_vect)
{
	// Turn on LED1
	LED1_ON

	// wait for some delay
	_delay_ms(CONF_DELAY);

	// Turn off LED1
	LED1_OFF
}

int main(void)
{
	system_init();
	LED0_OFF
	LED1_OFF
	// Configure INT0 to sense rising edge
	Configure_Interrupt(INTR0, RISING);

	// Enable INT0 interrupt
	Enable_Interrupt(INTR0);

	// Enable pin change interrupt PCINT0 on PORTB0
	Enable_Pcinterrupt(PCINTR0);

	// Enable global interrupt enable bit
	sei();

	while (1) {
	}
	return 0;
}
