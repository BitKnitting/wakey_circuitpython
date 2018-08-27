#include <atmel_start.h>
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
