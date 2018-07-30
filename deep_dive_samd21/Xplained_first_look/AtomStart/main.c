#include <atmel_start.h>
#pragma GCC push_options
#pragma GCC optimize ("O0")
int main(void)
{
	int i = 1;
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();

  i = 1 + 1;
}
#pragma GCC pop_options
