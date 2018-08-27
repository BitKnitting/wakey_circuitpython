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

static void cb_eic_extint3(void) {
	// gunk...checking if can set bp.
	uint8_t i,j;
	i = 5;
	j = i + 3;
	i = j + i;
}
int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();
  ext_irq_register(PIN_PA19,cb_eic_extint3);
	/* Replace with your application code */
	while (1) {
	}
}
