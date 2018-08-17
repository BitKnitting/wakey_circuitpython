#include <atmel_start.h>

void awake() {
	for (int i = 0;i < 4;i++) {
		delay_ms(200);
		gpio_toggle_pin_level(LED0);
	}
}
void zzz(void)
{
	__DSB(); // Complete any pending buffer writes.
	SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
	__WFI();
}
int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();
	ext_irq_register(WAKEY,awake);
	gpio_set_pin_level(LED0,true);
	/* Replace with your application code */
	while (1) {
		zzz();
	}
}
