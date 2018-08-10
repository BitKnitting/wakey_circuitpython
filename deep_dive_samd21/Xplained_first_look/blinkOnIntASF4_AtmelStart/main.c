#include <atmel_start.h>

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
  /* set up a callback to happen when the button is pressed */
	ext_irq_register(BUTTON,button_pressed);
	/* Replace with your application code */
	while (1) {

	}
}
