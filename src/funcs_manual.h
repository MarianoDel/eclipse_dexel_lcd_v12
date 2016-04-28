/*
 * funcs_manual.h
 *
 *  Created on: 27/04/2016
 *      Author: Mariano
 */

#ifndef FUNCS_MANUAL_H_
#define FUNCS_MANUAL_H_

//estados del modo MANUAL
enum var_manual_states
{

	MANUAL_INIT = 0,
	MANUAL_UPDATE,
	MANUAL_CH1,
	MANUAL_CH2,
	MANUAL_TO_SAVE

};

//estados de Menu Selections
#define MENU_ON			0
#define MENU_SELECTED	1
#define MENU_OFF		2
#define MENU_WAIT_FREE	3

//estados del menu MANUAL
#define MANUAL_MENU_INIT				0
#define MANUAL_MENU_CHANNELS			1
#define MANUAL_MENU_RELAYS				2
#define MANUAL_MENU_FINISH				3

#define MANUAL_MENU_CHANNELS_SELECTED	10
#define MANUAL_MENU_RELAYS_SELECTED		11
#define MANUAL_MENU_FINISH_SELECTED		12

//-------- Functions -------------
unsigned char FuncManual (void);
unsigned char MenuManual (void);

void FuncManualReset (void);
void MenuManualReset (void);


#endif /* FUNCS_MANUAL_H_ */
