/*
 * gpio_control.c
 *
 *  Created on: Mar 11, 2023
 *      Author: HP
 */

#include "xgpio.h"
#include "xil_types.h"
#include "xparameters.h" // contains address spaces and device IDs
#include "xil_exception.h" // contains exception functions
#include "xscugic.h" // contains drivers for config of the GIC (Global Interrupt Controller)
#include "gpio_control.h"

// To address a hardware peripheral we need to know the address range and device ID
// Define GPIO AND interrupt ID
#define GPIO_DEVICE_ID XPAR_XGPIOPS_0_DEVICE_ID
#define INTC_DEVICE_ID XPAR_SCUGIC_0_DEVICE_ID
#define GPIO_INTERRUPT_ID XPS_GPIO_INT_ID

#define XGPIOPS_IRQ_TYPE_EDGE_RISING 0

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

// Function to select state of LEDs based on the switches flag value
void check_state(){
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

// Check the initial state of the switches
int check_switches_initial(){
	// Read from switches
	// Return integer value of switches
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

// Function to toggle leds when any of the push buttons are pressed
void toggle_leds(){
	// Create new unsigned 32 variable
	// Gpio read on switches
	// XGpio_DiscreteRead(instance_of_gpio, channel)
	u32 buttons_check = XGpio_DiscreteRead(&axi_gpio_0, 2);
	XGpio_DiscreteWrite(&axi_gpio_0, 1, buttons_check);
	usleep(500);
}

// Blink LEDs ON for 500 ms
// Turn LEDs OFF for 500 ms
void blink_leds_normally(){
	XGpio_DiscreteWrite(&axi_gpio_0, 1, 0b00001111);
	usleep(500000);
	XGpio_DiscreteWrite(&axi_gpio_0, 1, 0b00000000);
	usleep(500000);
}

// Blinks LEDs in SOS pattern
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
		usleep(1500000);
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
	sleep(1);

}

// We need 2 functions for the interrupts
// intr_setup(), will setup the interrupt controller and interrupt for the AXI GPIO 1 block
// intr_handler(), interrupt subroutine, function that performs the switches_flag update
// Handles interrupt
void intr_handler(){
	// Disable the AXI GPIO interrupts
	// Prevent any other interrupts from the GPIO
	XGpio_InterruptDisable(&axi_gpio_1, XGPIO_IR_CH1_MASK);

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

// Interrupt configuration
// 1. Initialize interrupt controller
// 2. Interrupt enable on AXI GPIO
// 3. Connect the interrupt handler to the interrupt controller
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
