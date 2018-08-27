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
// #pragma GCC push_options
// #pragma GCC optimize ("O0")

void EXTERNAL_IRQ_0_init(void)
{
	_gclk_enable_channel(EIC_GCLK_ID, CONF_GCLK_EIC_SRC);
	// Set pin direction to input
	gpio_set_pin_direction(WAKEY, GPIO_DIRECTION_IN);

	gpio_set_pin_pull_mode(WAKEY,
	                       // <y> Pull configuration
	                       // <id> pad_pull_config
	                       // <GPIO_PULL_OFF"> Off
	                       // <GPIO_PULL_UP"> Pull-up
	                       // <GPIO_PULL_DOWN"> Pull-down
	                       GPIO_PULL_OFF);
	gpio_set_pin_function(WAKEY, PINMUX_PA19A_EIC_EXTINT3);

	ext_irq_init();
}

void system_init(void)
{
	init_mcu();

	EXTERNAL_IRQ_0_init();
}
// #pragma GCC pop_options
