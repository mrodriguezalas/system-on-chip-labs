/*
 * gpio_control.c
 *
 *  Created on: Mar 11, 2023
 *      Author: HP
 */

#include <stdio.h>
#include "platform.h"
#include "xgpio.h"
#include "xil_types.h"
#include "xparameters.h"
#include "gpio_control.h"

// Configuration variables for GPIO
XGpio_Config *cfg_ptr;
XGpio_Config *cfg_ptr2;
// GPIO instances
XGpio leds;
XGpio buttons;
XGpio switches;

// Configure, initialize and set data direction for GPIOs
void init_axi_gpio(){

	// Configure GPIO device at location 0 and location 1
	xil_printf("initialization and configuration of the AXI GPIO\n\r");
	cfg_ptr = XGpio_LookupConfig(XPAR_AXI_GPIO_0_DEVICE_ID);
	cfg_ptr2 = XGpio_LookupConfig(XPAR_AXI_GPIO_1_DEVICE_ID);

	// Initialize leds and buttons on GPIO 0
	// Initialize switches on GPIO 1
	XGpio_CfgInitialize(&leds, cfg_ptr, cfg_ptr->BaseAddress);
	XGpio_CfgInitialize(&buttons, cfg_ptr, cfg_ptr->BaseAddress);
	// Initialize GPIO module 2
	XGpio_CfgInitialize(&switches, cfg_ptr2, cfg_ptr2->BaseAddress);

	// Set data direction
	// Function XGpio_SetDataDirection() uses third parameter as input or output...
	// ... where bits set to 0 are outputs, bits set to 1 are inputs
	XGpio_SetDataDirection(&leds, 1, 0); // set data direction, channel 1 (leds), output mode
	XGpio_SetDataDirection(&buttons, 2, 1); // set data direction, channel 2 (buttons), input mode
	// Set switches
	// In this case the SetDataDirection may not be necessary, as only one channel of the...
	// ... gpio2 is active and switches can only work as inputs
	XGpio_SetDataDirection(&switches, 1, 1); // set data direction, channel 1, input mode
}

// Read from switches and select if blinking or toggling buttons
void check_switches(){
	// ===== Steps for reading from GPIO ====
	// 1. Create a GPIO instance
	// 2. Initialize de GPIO
	// 3. Set data direction
	// 4. Read the data

	// ===== Steps for writing to GPIO ====
	// 1. Create a GPIO instance
	// 2. Initialize the GPIO
	// 3. Write the data

	// Create new unsigned 32 variable
	// Gpio read on switches
	// XGpio_DiscreteRead(instance_of_gpio, channel)
	u32 switches_check = XGpio_DiscreteRead(&switches, 1);

	// Check if sw0 is on
	if(switches_check!=0b00000001){
		u32 buttons_check = XGpio_DiscreteRead(&buttons, 2);
		XGpio_DiscreteWrite(&leds, 1, buttons_check);
		usleep(500);

	// Else case for other switches binary representation, i.e. 0b00, 0b10, 0b11
	} else{
		// Turn on all leds on gpio channel 1
		// Sleep for 500 ms
		// Turn off all leds on gpio channel 1
		// Sleep for 500 ms
		XGpio_DiscreteWrite(&leds, 1, 0b00001111);
		usleep(500000);
		XGpio_DiscreteWrite(&leds, 1, 0b00000000);
		usleep(500000);
	}
}
