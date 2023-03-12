/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * lab 1.2 - Interrupts
 *
 * This application runs a simple interrupt tied to the GPIO1, which contains 1 channel...
 * ...with switches tied to it.
 *
 *
 */

#include <stdio.h>
#include "platform.h"
#include "xil_types.h"
#include "xil_printf.h"
#include "gpio_control.h"

int main(){

	xil_printf("Initialize platform\n\r");
	init_platform();

	// Initialize and configure both GPIOs
	xil_printf("Initialize GPIOs\n\r");
	init_axi_gpio();

	// Check the initial state of the switches
	// This is done to prevent unwanted states when starting the program
	// As the switches are not guaranteed to start both on OFF state
	switches_flag = check_switches_initial();
	xil_printf("%d", switches_flag);

	// Setup interrupt for AXI GPIO 1
	intr_setup();

	while(1){
		// Will check for the global switches_flag value and select the appropiate state
		// switches_flag value is only changed on the switches interrupt
		check_state();
	}

	// Cleanup platform
	xil_printf("Cleanup\n\r");
	cleanup_platform();
	return 0;
}
