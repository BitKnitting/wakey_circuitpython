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
	//_gclk_enable_channel(EIC_GCLK_ID, CONF_GCLK_EIC_SRC);

	// Set pin direction to input
	gpio_set_pin_direction(BUTTON, GPIO_DIRECTION_IN);

	gpio_set_pin_pull_mode(BUTTON,
	                       // <y> Pull configuration
	                       // <id> pad_pull_config
	                       // <GPIO_PULL_OFF"> Off
	                       // <GPIO_PULL_UP"> Pull-up
	                       // <GPIO_PULL_DOWN"> Pull-down
	                       GPIO_PULL_OFF);

	gpio_set_pin_function(BUTTON, PINMUX_PA15A_EIC_EXTINT15);
	//hri_eic_wait_for_sync(EIC);
	// if (hri_eic_get_CTRL_reg(EIC, EIC_CTRL_ENABLE)) {
	// 	hri_eic_write_CTRL_reg(EIC, 0);
	// 	hri_eic_wait_for_sync(EIC);
	// }
	// hri_eic_write_CTRL_reg(EIC, EIC_CTRL_SWRST);
	// hri_eic_wait_for_sync(EIC);
	// uint32_t extint_mask = 1 << 15;
	// EIC->WAKEUP.reg |= extint_mask;
	// uint8_t config_index = 15 / 8;
	// uint8_t position = (15 % 8) * 4;
	// EIC->CONFIG[config_index].reg &=~ (EIC_CONFIG_SENSE0_Msk << position);
	// EIC->CONFIG[config_index].reg |= EIC_CONFIG_SENSE0_HIGH_Val << position;
	// hri_eic_set_CTRL_ENABLE_bit(EIC);
	//
	// NVIC_DisableIRQ(EIC_IRQn);
	// NVIC_ClearPendingIRQ(EIC_IRQn);
	// NVIC_EnableIRQ(EIC_IRQn);
	ext_irq_init();
}

void delay_driver_init(void)
{
	delay_init(SysTick);
}

void system_init(void)
{
	init_mcu();

	// GPIO on PB30

	// Set pin direction to output
	gpio_set_pin_direction(LED0, GPIO_DIRECTION_OUT);

	gpio_set_pin_level(LED0,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   true);

	gpio_set_pin_function(LED0, GPIO_PIN_FUNCTION_OFF);

	EXTERNAL_IRQ_0_init();

	delay_driver_init();
}
