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
#pragma GCC push_options
#pragma GCC optimize ("O0")

static void (*myCallback)(void);


void EIC_Handler(void)
{
  EIC->INTFLAG.reg = (1 << PIN_PA19A_EIC_EXTINT_NUM) << EIC_INTFLAG_EXTINT_Pos;
	ASSERT(myCallback);
	myCallback();
}

void register_callback(void (*cb)(void))
{
 myCallback = cb;
}

void init_EIC()
{
	/*
	 * Set the interrupt to trigger when the the voltage rises from LOW to HIGH.
	 */
	uint8_t config_index = PIN_PA19A_EIC_EXTINT_NUM / 8;
	uint8_t position = (PIN_PA19A_EIC_EXTINT_NUM % 8) * 4;
	// First clear the CONFIG register (in this case CONFIG0).
	EIC->CONFIG[config_index].reg &=~(EIC_CONFIG_SENSE0_Msk << position);
	// Then set to rising with filtering.
	uint32_t sense_setting = EIC_CONFIG_SENSE0_RISE_Val | EIC_CONFIG_FILTEN0;
	EIC->CONFIG[config_index].reg |= sense_setting << position;
	/* Configure the EIC so that the SamD21 will wake up from standby
	* when the external interrupt assigned to PA19 is triggered.
	*/
	uint32_t extint_mask = 1 << PIN_PA19A_EIC_EXTINT_NUM;
	EIC->WAKEUP.reg |= extint_mask;
	// Enable the Interrupt.
	hri_eic_set_INTEN_reg(EIC, extint_mask);
	// Config NVIC
	NVIC_DisableIRQ(EIC_IRQn);
	NVIC_ClearPendingIRQ(EIC_IRQn);
	NVIC_EnableIRQ(EIC_IRQn);
	// Turn the EIC peripheral on.
	hri_eic_set_CTRL_ENABLE_bit(EIC);
}


void EXTERNAL_IRQ_0_init(void)
{
	_gclk_enable_channel(EIC_GCLK_ID, CONF_GCLK_EIC_SRC);
	// Set pin direction to input
	gpio_set_pin_direction(PA19, GPIO_DIRECTION_IN);

	gpio_set_pin_pull_mode(PA19,
	                       // <y> Pull configuration
	                       // <id> pad_pull_config
	                       // <GPIO_PULL_OFF"> Off
	                       // <GPIO_PULL_UP"> Pull-up
	                       // <GPIO_PULL_DOWN"> Pull-down
	                       GPIO_PULL_DOWN);

	gpio_set_pin_function(PA19, PINMUX_PA19A_EIC_EXTINT3);

	init_EIC();
}

void system_init(void)
{
	init_mcu();

	EXTERNAL_IRQ_0_init();
}
#pragma GCC pop_options
