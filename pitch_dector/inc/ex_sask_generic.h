/*****h*        examples/Timer/Timer_Delay/h/ex_timer.h
  * Summary:
  *         Library of functions to time things
  *
  * Variables:
  *     none
  * Functions:
  *		ex_timer_init( clock_frequency )
  *		ex_timer_wait( time_in_s )

  * Notes:
  *		none

  * ToDo:
  *     none

  * Originator:
  *     Chris Bore, BORES Signal processing, chris@bores.com, www.bores.com

  * History:
  *      Version 1.00     13/03/2013
  *****/

#ifndef __EX_SASK_GENERIC_H__
#define __EX_SASK_GENERIC_H__

_FGS(GWRP_OFF & GCP_OFF);
_FOSCSEL(FNOSC_FRC);
_FOSC(FCKSM_CSECMD & OSCIOFNC_ON & POSCMD_NONE);
_FWDT(FWDTEN_OFF);

void ex_sask_init( void );

#endif
