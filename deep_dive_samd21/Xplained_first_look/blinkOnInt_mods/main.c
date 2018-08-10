#include <atmel_start.h>
#include <hri_eic_d21.h>
void button_pressed() {
	for (int i = 0;i < 8;i++) {
		gpio_toggle_pin_level(LED0);
				delay_ms(200);
	}
}
int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();
  /* set up a callback to happen when the button is pressed */
	hri_eic_set_INTEN_reg(EIC, 1ul << 15);
	ext_irq_register(BUTTON,button_pressed);
	/* Replace with your application code */
	while (1) {
		__DSB(); // Complete any pending buffer writes.
		SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
		__WFI();
		//button_pressed();

	}
}
