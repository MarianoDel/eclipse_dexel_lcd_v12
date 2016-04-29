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

//estados del menu DMX
#define DMX_MENU_INIT				0
#define DMX_MENU_ADDRESS			1
#define DMX_MENU_CHANNELS			2
#define DMX_MENU_RELAYS				3
#define DMX_MENU_FINISH				4

#define DMX_MENU_ADDRESS_SELECTED		10
#define DMX_MENU_ADDRESS_SELECTED_1		11
#define DMX_MENU_ADDRESS_SELECTED_2		12
#define DMX_MENU_CHANNELS_SELECTED		13
#define DMX_MENU_CHANNELS_SELECTED_1	14
#define DMX_MENU_CHANNELS_SELECTED_2	15
#define DMX_MENU_RELAYS_SELECTED		16
#define DMX_MENU_RELAYS_SELECTED_1		17
#define DMX_MENU_RELAYS_SELECTED_2		18
#define DMX_MENU_FINISH_SELECTED		19

//-------- Functions -------------
unsigned char FuncDMX (void);
unsigned char MenuDMX (void);

void FuncDMXReset (void);
void MenuDMXReset (void);


#endif /* FUNCS_DMX_H_ */
