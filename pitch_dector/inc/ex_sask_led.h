 /*****h*        examples/Timer/Timer_Delay/h/ex_led.h
  * Summary:
  *         Library of functions to control SASK LEDs
  *
  * Variables:
  *     none
  * Functions:
  *		led_on( color )
  *		led_off( color )
  *		led_switch( color )			switch LED state

  * Notes:
  *		none

  * ToDo:
  *     none

  * Originator:
  *     Chris Bore, BORES Signal processing, chris@bores.com, www.bores.com

  * History:
  *      Version 1.00     13/03/2013
  *****/
#ifndef __EX_LED_H__
#define __EX_LED_H__

#define RED 1;
#define YELLOW 2;
#define GREEN 3;

void led_on( short int );
void led_off( short int );
void led_switch( short int );
void led_select( short int );

#endif
