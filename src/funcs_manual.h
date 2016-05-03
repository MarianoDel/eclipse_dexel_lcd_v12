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
	MANUAL_CH1_1,
	MANUAL_CH2,
	MANUAL_CH2_1,
	MANUAL_GOING_CH2,
	MANUAL_GOING_CH1,
	MANUAL_GOING_UP,
	MANUAL_TO_SAVE

};

//estados de Menu Selections
#define MENU_M_ON			0
#define MENU_M_OFF		1

//estados del menu MANUAL
#define MANUAL_MENU_INIT				0
#define MANUAL_MENU_CHANNELS_0			1
#define MANUAL_MENU_CHANNELS_1			2
#define MANUAL_MENU_CHANNELS_2			3


#define TT_UPDATE_BUTTON 80
#define TT_UPDATE_BUTTON_SPEED 10

//-------- Functions -------------
unsigned char FuncManual (unsigned char);
unsigned char MenuManual (void);

void FuncManualReset (void);
void MenuManualReset (void);


#endif /* FUNCS_MANUAL_H_ */
