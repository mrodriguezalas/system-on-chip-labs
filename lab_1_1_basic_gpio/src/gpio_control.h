/*
 * gpio_control.h
 *
 *  Created on: Mar 11, 2023
 *      Author: HP
 */

#ifndef SRC_GPIO_CONTROL_H_
#define SRC_GPIO_CONTROL_H_

// --------------- Function prototypes  ---------------
/*
 * init_axi_gpio()
 * Configure, Initialize and set data direction for GPIOs
 * @return void*/
void init_axi_gpio();
/*
 * init_axi_gpio()
 * Will read GPIO from the buttons and will inmediatly write the value to the LEDs
 * @return void*/
void check_switches();

#endif /* SRC_GPIO_CONTROL_H_ */
