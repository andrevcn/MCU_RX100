#ifndef __BUTTON_H
#define __BUTTON_H

/* Inclusoes de arquivos de cabecalho ----------------------------------------*/
#include "r_cg_main.h"
#include "r_cg_userdefine.h"
#include "r_gpio_rx111.h"


#define CLICK_EVENT 		1
#define DOUBLE_CLICK_EVENT	2
#define HOLD_EVENT 			3
#define LONG_HOLD_EVENT		4

typedef struct {
	uint8_t 	debounce;          	// ms debounce period to prevent flickering when pressing or releasing the button
	uint8_t 	DCgap;            	// max ms between clicks for a double click event
	uint16_t 	holdTime;        	// ms hold period: how long to wait for press+hold event
	uint16_t 	longHoldTime;    	// ms long hold period: how long to wait for press+hold event
	uint8_t 	buttonVal;   		// value read from button
	uint8_t 	buttonLast;  		// buffered value of the button's previous state
	uint8_t 	DCwaiting;  		// whether we're waiting for a double click (down)
	uint8_t 	DConUp;     		// whether to register a double click on next release, or whether to wait and click
	uint8_t 	singleOK;    		// whether it's OK to do a single click
	long 		downTime;         	// time the button was pressed down
	long 		upTime;           	// time the button was released
	uint8_t 	ignoreUp;   		// whether to ignore the button release because the click+hold was triggered
	uint8_t 	waitForUp;        	// when held, whether to wait for the up event
	uint8_t 	holdEventPast;    	// whether or not the hold event happened already
	uint8_t 	longHoldEventPast;	// whether or not the long hold event happened already
	uint8_t	    event;
	gpio_port_pin_t pin;
}Button_t;

void Button_Init(Button_t *Button, gpio_port_pin_t pin);



#endif /*__BUTTON_H */


