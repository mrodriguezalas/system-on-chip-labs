/*

 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
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
