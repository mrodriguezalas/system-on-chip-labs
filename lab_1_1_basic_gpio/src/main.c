/*
 * lab 1.1 - Interrupts
 *
 * This application runs a simple interrupt tied to the GPIO1, which contains 1 channel...
 * ...with switches tied to it.
 *
 *
 */

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "gpio_control.h"

int main()
{

	// Initialize xilinx platform
	xil_printf("initialization of the platform\n\r");
	init_platform();

	init_axi_gpio();

	// Repeat forever
	while(1) {
		check_switches();
	}

	// Cleanup code
	xil_printf("cleanup\n\r");
	cleanup_platform();
	return 0;
}
