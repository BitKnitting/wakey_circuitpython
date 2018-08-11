#include <atmel_start.h>
void gpio_init(void);
void zzz(void)
{
	__DSB(); // Complete any pending buffer writes.
	SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
	__WFI();
}
void button_pressed() {
	for (int i = 0;i < 4;i++) {
		delay_ms(200);
		gpio_toggle_pin_level(LED0);
	}
}
int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();
	gpio_init();
	ext_irq_init();
	/*********************************************************/
	/* set up a callback to happen when the button is pressed */
	//ext_irq_register(BUTTON,button_pressed);
	ext_irq_register(BUTTON,NULL);
	/* Replace with your application code */
	while (1) {
		zzz();
		button_pressed();
		}
	}

	/*********************************************************/
	/* Initial GPIO pin being used for interrupt */
//	_gclk_enable_channel(EIC_GCLK_ID, CONF_GCLK_EIC_SRC);

	// Set pin direction to input
	void gpio_init(){
		gpio_set_pin_direction(BUTTON, GPIO_DIRECTION_IN);

		gpio_set_pin_pull_mode(BUTTON,
		                       // <y> Pull configuration
		                       // <id> pad_pull_config
		                       // <GPIO_PULL_OFF"> Off
		                       // <GPIO_PULL_UP"> Pull-up
		                       // <GPIO_PULL_DOWN"> Pull-down
																									GPIO_PULL_UP);

		gpio_set_pin_function(BUTTON, PINMUX_PA15A_EIC_EXTINT15);
}
