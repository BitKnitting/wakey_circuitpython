#include <atmel_start.h>
#pragma GCC push_options
#pragma GCC optimize ("O0")
int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();
	int i = 5;
	i = i + 5;

	/* Replace with your application code */
	while (1) {
		delay_ms(500);
		gpio_toggle_pin_level(LED1);
	}
}
#pragma GCC pop_options
