/*
 * gpio_control.h
 *
 *  Created on: Mar 11, 2023
 *      Author: HP
 */

#ifndef SRC_GPIO_CONTROL_H_
#define SRC_GPIO_CONTROL_H_

// ---------------- Global Flags ----------------
// switches_flag - Saves the value of the switches
// this value is later used to select the appropiate case and behaviour of the LEDs
int switches_flag;
// ---------------- Function prototypes ----------------
/*
 * init_axi_gpio()
 * Configure, Initialize and set data direction for GPIOs
 * @return void*/
void init_axi_gpio();
/*
 * check_state()
 * Selects appropiate case for main function. Will select between...
 * ...toggle_leds(), blink_leds_normally(), blink_leds_sos()
 * @return void*/
void check_state();
/*
 * check_switches_initial()
 * Will be called once before the while loop, to check the initial state of the switches...
 * ...and set the initial condition to be passed to the check_state() function
 * @return switches_flag - the current value of the switches*/
int check_switches_initial();
/*
 * toggle_leds()
 * Will read GPIO from the buttons and will inmediatly write the value to the LEDs
 * @return void*/
void toggle_leds();
/*
 * blink_leds_normally()
 * Will trigger the blinking LEDs, ON for 500 ms and OFF for 500 ms
 * @return void*/
void blink_leds_normally();
/*
 * blink_leds_sos()
 * Will trigger the SOS blinking pattern. Blinking 3 times short, 3 times long, 3 times short.
 * Short pulses are 500 ms and long pulses are 1500 ms
 * @return void*/
void blink_leds_sos();
/*
 * intr_handler()
 * Will handle the interrupt from the switches.
 * This function will also read from the switches and update the switches_flag variable
 * @return void*/
void intr_handler();
/*
 * intr_handler()
 * Will initialize the interrupt controller, enable interrupt on GPIO and connect...
 * ...the interrupt handler to the interrupt controller.
 * @return void*/
void intr_setup();

#endif /* SRC_GPIO_CONTROL_H_ */
