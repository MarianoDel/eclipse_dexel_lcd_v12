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
	COLORS_3200,
	COLORS_4500,
	COLORS_5600,
	COLORS_WAIT_TO_GO,
	COLORS_GOING_UP

};

//estados de Menu Selections
#define MENU_ON			0
#define MENU_SELECTED	1
#define MENU_OFF		2
#define MENU_WAIT_FREE	3

//estados del menu COLORS
#define COLORS_MENU_INIT				0
#define COLORS_MENU_SELECTED			1
#define COLORS_MENU_SELECTED_1			2
#define COLORS_MENU_SELECTED_2			3


//-------- Functions -------------
unsigned char FuncColors (void);
unsigned char MenuColors (void);

void FuncColorsReset (void);
void MenuColorsReset (void);




#endif /* FUNCS_COLORS_H_ */
