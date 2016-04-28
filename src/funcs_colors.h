/*
 * funcs_colors.h
 *
 *  Created on: 28/04/2016
 *      Author: Mariano
 */

#ifndef FUNCS_COLORS_H_
#define FUNCS_COLORS_H_

//estados del modo COLORS
enum var_colors_states
{

	COLORS_INIT = 0,
	COLORS_UPDATE,
	COLORS_CH1,
	MANUAL_CH1_1,
	MANUAL_CH2,
	MANUAL_CH2_1,
	MANUAL_GOING_CH2,
	MANUAL_GOING_CH1,
	MANUAL_GOING_UP,
	MANUAL_TO_SAVE

};

//estados de Menu Selections
#define MENU_ON			0
#define MENU_SELECTED	1
#define MENU_OFF		2
#define MENU_WAIT_FREE	3

//estados del menu COLORS
#define COLORS_MENU_INIT				0
#define COLORS_MENU_CHANNELS			1
#define COLORS_MENU_RELAYS				2
#define COLORS_MENU_FINISH				3

#define MANUAL_MENU_CHANNELS_SELECTED	10
#define MANUAL_MENU_CHANNELS_SELECTED_1	11
#define MANUAL_MENU_CHANNELS_SELECTED_2	12
#define MANUAL_MENU_RELAYS_SELECTED		13
#define MANUAL_MENU_RELAYS_SELECTED_1	14
#define MANUAL_MENU_RELAYS_SELECTED_2	15
#define MANUAL_MENU_FINISH_SELECTED		16

//-------- Functions -------------
unsigned char FuncManual (void);
unsigned char MenuManual (void);

void FuncManualReset (void);
void MenuManualReset (void);




#endif /* FUNCS_COLORS_H_ */
