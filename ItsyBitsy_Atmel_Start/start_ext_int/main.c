#include <atmel_start.h>
#pragma GCC push_options
#pragma GCC optimize ("O0")
static void time_to_sleep(void){
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
  register_callback(cb_eic_extint3);
  //ext_irq_register(PIN_PA19,cb_eic_extint3);
	/* Replace with your application code */
	while (1) {
		time_to_sleep();
	}
}
#pragma GCC pop_options
