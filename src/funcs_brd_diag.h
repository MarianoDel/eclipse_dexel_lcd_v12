/*
 * funcs_brd_diag.h
 *
 *  Created on: 29/04/2016
 *      Author: Mariano
 */

#ifndef FUNCS_BRD_DIAG_H_
#define FUNCS_BRD_DIAG_H_

//estados del modo COLORS
enum var_brd_diag_states
{

	BRD_DIAG_INIT = 0,
	BRD_DIAG_LED_TEMP,
	BRD_DIAG_LED_TEMP_1,
	BRD_DIAG_BRD_TEMP,
	BRD_DIAG_FAN_SPEED,
	BRD_DIAG_SAVED_TIMES

};

//-------- Functions -------------
unsigned char FuncBrdDiag (void);

void FuncBrdDiagReset (void);



#endif /* FUNCS_BRD_DIAG_H_ */
