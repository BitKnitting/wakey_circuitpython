/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */

#include "driver_init.h"
#include <peripheral_clk_config.h>
#include <utils.h>
#include <hal_init.h>
#include <hpl_gclk_base.h>
#include <hpl_pm_base.h>

void EXTERNAL_IRQ_0_init(void)
{
	// Set pin direction to input
	gpio_set_pin_direction(WAKEY, GPIO_DIRECTION_IN);

	gpio_set_pin_pull_mode(WAKEY,
	                       // <y> Pull configuration
	                       // <id> pad_pull_config
	                       // <GPIO_PULL_OFF"> Off
	                       // <GPIO_PULL_UP"> Pull-up
	                       // <GPIO_PULL_DOWN"> Pull-down
	                       GPIO_PULL_DOWN);

	gpio_set_pin_function(WAKEY, PINMUX_PA19A_EIC_EXTINT3);

	ext_irq_init();
}

void system_init(void)
{
	init_mcu();

	// GPIO on PA17

	// Set pin direction to output
	gpio_set_pin_direction(LED0, GPIO_DIRECTION_OUT);

	gpio_set_pin_level(LED0,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   false);

	gpio_set_pin_function(LED0, GPIO_PIN_FUNCTION_OFF);

	EXTERNAL_IRQ_0_init();
}
