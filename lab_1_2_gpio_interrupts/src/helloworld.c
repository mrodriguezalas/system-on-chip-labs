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
 * helloworld.c: simple test application
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
#include "xgpio.h"
#include "xil_types.h"
#include "xparameters.h" // contains address spaces and device IDs
#include "xil_exception.h" // contains exception functions
#include "xscugic.h" // contains drivers for config of the GIC (Global Interrupt Controller)

// To address a hardware peripheral we need to know the address range and device ID
// Define GPIO AND interrupt ID
#define GPIO_DEVICE_ID XPAR_XGPIOPS_0_DEVICE_ID
#define INTC_DEVICE_ID XPAR_SCUGIC_0_DEVICE_ID
#define GPIO_INTERRUPT_ID XPS_GPIO_INT_ID

#define XGPIOPS_IRQ_TYPE_EDGE_RISING 0

// ---------------- Function prototypes ----------------
static void init_axi_gpio();
static int check_switches_initial();
static void toggle_leds();
static void blink_leds_normally();
static void blink_leds_sos();
static void intr_handler();
static void intr_setup();

// We need 2 functions for the interrupts
// intr_setup(), will setup the interrupt controller and interrupt for the AXI GPIO 1 block
// intr_handler(), interrupt subroutine, function that performs the SOS blinking

// Interrupt configuration
// 1. Initialize interrupt controller
// 2. Interrupt enable on AXI GPIO
// 3. Connect the interrupt handler to the interrupt controller

// AXI GPIO
XGpio_Config *axi_gpio_cfg_ptr;
XGpio_Config *axi_gpio_cfg_ptr_2;
// AXI GPIO 0 has leds and buttons
// AXI GPIO 1 has switches
XGpio axi_gpio_0;
XGpio axi_gpio_1;
// Interrupt configuration and controller instance
XScuGic_Config *intr_controller_cfg_ptr;
XScuGic intr_controller;

// ---------------- Global Flags ----------------
u8 switches_flag;

int main(){

	xil_printf("Initialize platform\n\r");
	init_platform();

	xil_printf("Initialize GPIOs\n\r");
	init_axi_gpio();

	switches_flag = check_switches_initial();
	xil_printf("%d", switches_flag);

	// Setup interrupt for AXI GPIO 1
	intr_setup();

	while(1){
		// Evaluate using the global switch value
		// Will check if the value of the switches has changed...
		// ...using the interrupt
		xil_printf("%d", switches_flag, "\n");
		switch(switches_flag){
		// When both switches are off
		case 0:
			// Toggle the leds
			toggle_leds();
			break;
		// If switch 0 is on
		// Blink leds constantly
		case 1:
			blink_leds_normally();
			break;
		// If switch 2 is on
		// Blink leds with SOS signal
		case 2:
			blink_leds_sos();
			break;
		// If both switches are on
		// Toggle leds
		case 3:
			// Toggle the leds
			toggle_leds();
			break;
		default:
			xil_printf("Something is not right");
		}
	}

	xil_printf("Cleanup\n\r");
	cleanup_platform();
	return 0;
}

int check_switches_initial(){
	// Read from switches
	switches_flag = XGpio_DiscreteRead(&axi_gpio_1, 1);
	xil_printf("%d", switches_flag);
	return switches_flag;
}

// Initialize and configure GPIOs
void init_axi_gpio(){
	xil_printf("Setting up GPIO\n\r");
	// Configure GPIO device at location 0 and location 1
	axi_gpio_cfg_ptr = XGpio_LookupConfig(XPAR_AXI_GPIO_0_DEVICE_ID);
	axi_gpio_cfg_ptr_2 = XGpio_LookupConfig(XPAR_AXI_GPIO_1_DEVICE_ID);

	// Initialize leds and buttons on GPIO 0
	XGpio_CfgInitialize(&axi_gpio_0, axi_gpio_cfg_ptr, axi_gpio_cfg_ptr->BaseAddress);
	// Initialize switches on GPIO 1
	XGpio_CfgInitialize(&axi_gpio_1, axi_gpio_cfg_ptr_2, axi_gpio_cfg_ptr_2->BaseAddress);
	// Set data direction
	// Function XGpio_SetDataDirection() uses third parameter as input or output...
	// ... where bits set to 0 are outputs, bits set to 1 are inputs
	XGpio_SetDataDirection(&axi_gpio_0, 1, 0); // set data direction, channel 1 (leds), output mode
	XGpio_SetDataDirection(&axi_gpio_0, 2, 1); // set data direction, channel 2 (buttons), input mode
	// Set switches
	// In this case the SetDataDirection may not be necessary, as only one channel of the...
	// ... gpio2 is active and switches can only work as inputs
	XGpio_SetDataDirection(&axi_gpio_1, 1, 1); // set data direction, channel 1, input mode
	xil_printf("Finished setup\n\r");
	return;
}

// Function to toggle leds when button is pressed
void toggle_leds(){
	// Create new unsigned 32 variable
	// Gpio read on switches
	// XGpio_DiscreteRead(instance_of_gpio, channel)
	u32 buttons_check = XGpio_DiscreteRead(&axi_gpio_0, 2);
	XGpio_DiscreteWrite(&axi_gpio_0, 1, buttons_check);
	usleep(500);
}

void blink_leds_normally(){
	XGpio_DiscreteWrite(&axi_gpio_0, 1, 0b00001111);
	usleep(500000);
	XGpio_DiscreteWrite(&axi_gpio_0, 1, 0b00000000);
	usleep(500000);
}

void blink_leds_sos(){
	// Short short short
	// This is a blocking execution
	// Holds the program until it has finished blinking
	// Maybe add a if to check if the switches has changed??
	for (int i = 0; i < 3; i++){
		XGpio_DiscreteWrite(&axi_gpio_0, 1, 0b00001111);
		usleep(500000);
		XGpio_DiscreteWrite(&axi_gpio_0, 1, 0b00000000);
		usleep(500000);
	}
	// Long long long
	for (int j = 0; j < 3; j++){
		XGpio_DiscreteWrite(&axi_gpio_0, 1, 0b00001111);
		sleep(1);
		XGpio_DiscreteWrite(&axi_gpio_0, 1, 0b00000000);
		usleep(500000);
	}

	// Short short short
	for (int k = 0; k < 3; k++){
		XGpio_DiscreteWrite(&axi_gpio_0, 1, 0b00001111);
		usleep(500000);
		XGpio_DiscreteWrite(&axi_gpio_0, 1, 0b00000000);
		usleep(500000);
	}
	sleep(2);

}

void intr_handler(){
	// Disable the AXI GPIO interrupts
	// Prevent any other interrupts from the GPIO
	XGpio_InterruptDisable(&axi_gpio_1, XGPIO_IR_CH1_MASK);

	// TO DO
	// Add code for interrupt handler
	xil_printf("\nInterrupt from switches\n\r");
	u32 switches_check = XGpio_DiscreteRead(&axi_gpio_1, 1);

	switch(switches_check){
	case 0:
		switches_flag = 0;
		break;
	case 1:
		switches_flag = 1;
		break;
	case 2:
		switches_flag = 2;
		break;
	case 3:
		switches_flag = 3;
		break;
	default:
		xil_printf("Something is not right\n\r");

	}
	// Clear the interrupt
	(void) XGpio_InterruptClear(&axi_gpio_1, XGPIO_IR_CH1_MASK);
	// Re enable the interrupt
	XGpio_InterruptEnable(&axi_gpio_1, XGPIO_IR_CH1_MASK);
}

// Function to setup interrupts
void intr_setup(){
	// Initialize the interrupt controller
	intr_controller_cfg_ptr = XScuGic_LookupConfig(XPAR_PS7_SCUGIC_0_DEVICE_ID);
	XScuGic_CfgInitialize(&intr_controller, intr_controller_cfg_ptr, intr_controller_cfg_ptr->CpuBaseAddress);

	// Enable the interrupt on the AXI GPIO 1
	XGpio_InterruptEnable(&axi_gpio_1, XGPIO_IR_CH1_MASK);
	XGpio_InterruptGlobalEnable(&axi_gpio_1);

	// Connect the interrupt controller to the interrupt hardware
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, (Xil_ExceptionHandler) XScuGic_InterruptHandler, &intr_controller);
	Xil_ExceptionEnable();

	// Connect the AXI GPIO interrupt to the interrupt controller
	XScuGic_Connect(&intr_controller, XPAR_FABRIC_AXI_GPIO_1_IP2INTC_IRPT_INTR, (Xil_ExceptionHandler) intr_handler, &axi_gpio_1);

	// enable the AXI GPIO interrupt at the interrupt controller
	XScuGic_Enable(&intr_controller, XPAR_FABRIC_AXI_GPIO_1_IP2INTC_IRPT_INTR);
}
