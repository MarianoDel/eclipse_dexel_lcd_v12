/*
 * funcs_dmx.h
 *
 *  Created on: 28/04/2016
 *      Author: Mariano
 */

#ifndef FUNCS_DMX_H_
#define FUNCS_DMX_H_

//estados del modo DMX
enum var_dmx_states
{

	DMX_INIT = 0,
	DMX_UPDATE,
	DMX_CH1,
	DMX_CH1_1,
	DMX_CH2,
	DMX_CH2_1,
	DMX_GOING_CH2,
	DMX_GOING_CH1,
	DMX_GOING_UP,
	DMX_TO_SAVE

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


#endif /* FUNCS_DMX_H_ */
