/*
 * funcs_colors.c
 *
 *  Created on: 28/04/2016
 *      Author: Mariano
 */

#include "funcs_colors.h"
#include "hard.h"
#include "main_menu.h"
#include "dmx_transceiver.h"
#include "lcd.h"

#include <stdio.h>
#include <string.h>


/* Externals variables ---------------------------------------------------------*/
extern volatile unsigned short function_timer;
extern volatile unsigned short function_enable_menu_timer;

#define colors_timer function_timer
#define colors_enable_menu_timer function_enable_menu_timer

extern volatile unsigned short lcd_backlight_timer;

extern const char s_blank_line [];

extern Configuration_Typedef ConfStruct_local;

/* Global variables ------------------------------------------------------------*/
enum var_colors_states colors_state = COLORS_INIT;
unsigned char colors_selections = 0;
unsigned char colors_menu_state = 0;
//unsigned short manual_ii = 0;

const unsigned char s_sel_colors [] = { 0x05, 0x0e, 0x45, 0x4e };


//-------- Functions -------------

//muestro el menu y hago funciones (pero sin mostrar nada) hasta que pasen 30 segs de TT_MENU_ENABLED
//con func_menu_show
unsigned char FuncColors (void)
{
	unsigned char resp = RESP_CONTINUE;
	unsigned char resp_down = RESP_CONTINUE;

	switch (colors_state)
	{
		case COLORS_INIT:
			//apago DMX
			DMX_Disa();
			MenuColorsReset();
			colors_state = COLORS_UPDATE;
			if (colors_enable_menu_timer)
				colors_selections = MENU_ON;
			break;

		case COLORS_UPDATE:
			if (colors_selections == MENU_OFF)
			{
				if (ConfStruct_local.colors_selected == 0)
					colors_state = COLORS_3200;
				else if (ConfStruct_local.colors_selected == 1)
					colors_state = COLORS_4500;
				else
					colors_state = COLORS_5600;

				LCD_1ER_RENGLON;
				LCDTransmitStr((const char *) " DEXEL LIGHTING ");

			}
			break;

		case COLORS_3200:
			LCD_2DO_RENGLON;
			LCDTransmitStr((const char *) " Color in 3200K ");
			Update_TIM3_CH1 (255);
			Update_TIM3_CH2 (0);

			colors_state = COLORS_WAIT_TO_GO;
			break;

		case COLORS_4500:
			LCD_2DO_RENGLON;
			LCDTransmitStr((const char *) " Color in 4500K ");
			Update_TIM3_CH1 (127);
			Update_TIM3_CH2 (127);

			colors_state = COLORS_WAIT_TO_GO;
			break;

		case COLORS_5600:
			LCD_2DO_RENGLON;
			LCDTransmitStr((const char *) " Color in 5600K ");
			Update_TIM3_CH1 (0);
			Update_TIM3_CH2 (255);

			colors_state = COLORS_WAIT_TO_GO;
			break;

		case COLORS_WAIT_TO_GO:
			if (CheckSSel() > S_HALF)
			{
				LCD_1ER_RENGLON;
				LCDTransmitStr((const char *) " Going Menu UP  ");
				LCD_2DO_RENGLON;
				LCDTransmitStr(s_blank_line);

				colors_state = COLORS_GOING_UP;
			}

			break;

		case COLORS_GOING_UP:
			//Subo un piso el MENU
			if (CheckSSel() == S_NO)
			{
				MenuColorsReset();
				colors_selections = MENU_ON;
				colors_state = COLORS_UPDATE;
				colors_enable_menu_timer = TT_MENU_ENABLED;
			}
			break;

		default:
			colors_state = COLORS_INIT;
			break;
	}

	//veo el de configuracion hasta TT_MENU_ENABLED
	switch (colors_selections)
	{
		case MENU_ON:

			resp_down = MenuColors();

			if (resp_down == RESP_WORKING)	//alguien esta tratando de seleccionar algo, le doy tiempo
				colors_enable_menu_timer = TT_MENU_ENABLED;

			if (resp_down == RESP_SELECTED)	//se selecciono algo
			{
				colors_enable_menu_timer = TT_MENU_ENABLED;
				colors_selections = MENU_SELECTED;
			}

			if (!colors_enable_menu_timer)	//ya mostre el menu mucho tiempo, lo apago
			{
				LCD_1ER_RENGLON;
				LCDTransmitStr((const char *)s_blank_line);
				LCD_2DO_RENGLON;
				LCDTransmitStr((const char *)s_blank_line);
				colors_selections = MENU_OFF;
			}

			if (resp_down == RESP_FINISH)	//se terminaron las selecciones
			{
				colors_enable_menu_timer = 0;
				LCD_1ER_RENGLON;
				LCDTransmitStr((const char *)s_blank_line);
				LCD_2DO_RENGLON;
				LCDTransmitStr((const char *)s_blank_line);

				colors_selections = MENU_OFF;
			}

			if (resp_down == RESP_CHANGE_ALL_UP)	//se terminaron las selecciones
				resp = RESP_CHANGE_ALL_UP;

			break;

		case MENU_SELECTED:
			//estado algo seleccionado espero update
			resp_down = FuncShowBlink ((const char *) "Something Select", (const char *) "Updating Values", 1, BLINK_NO);

			if (resp_down == RESP_FINISH)
			{
				colors_state = COLORS_UPDATE;
				colors_selections = MENU_ON;
			}
			break;

		case MENU_OFF:

			break;

		case MENU_WAIT_FREE:
			if (CheckSSel() == S_NO)
			{
				colors_selections = MENU_ON;
				//voy a activar el Menu
				//me fijo en ue parte del Menu estaba
				//TODO ES UNA CHANCHADA, PERO BUENO...

				/*
				if (standalone_menu_state <= STAND_ALONE_MENU_RAMP_ON_DIMMING)
				{
					//salgo directo
					LCD_2DO_RENGLON;
					LCDTransmitStr((const char *) "Cont.     Select");
				}
				else
				{
					if (standalone_menu_state <= STAND_ALONE_MENU_MOV_SENS_SELECTED_2)
					{
						standalone_menu_state = STAND_ALONE_MENU_MOV_SENS;
					}
					else if (standalone_menu_state <= STAND_ALONE_MENU_LDR_SELECTED_5)
					{
						standalone_menu_state = STAND_ALONE_MENU_LDR;
					}
					else if (standalone_menu_state <= STAND_ALONE_MENU_MAX_DIMMING_SELECTED_1)
					{
						standalone_menu_state = STAND_ALONE_MENU_MAX_DIMMING;
					}
					else if (standalone_menu_state <= STAND_ALONE_MENU_MIN_DIMMING_SELECTED_1)
					{
						standalone_menu_state = STAND_ALONE_MENU_MIN_DIMMING;
					}
					else if (standalone_menu_state <= STAND_ALONE_MENU_RAMP_ON_START_SELECTED_1)
					{
						standalone_menu_state =STAND_ALONE_MENU_RAMP_ON_START;
					}
					else if (standalone_menu_state <= STAND_ALONE_MENU_RAMP_ON_DIMMING_SELECTED_1)
					{
						standalone_menu_state = STAND_ALONE_MENU_RAMP_ON_DIMMING;
					}
					FuncOptionsReset ();
					FuncShowSelectv2Reset ();
					FuncChangeReset ();
				}
				*/
			}
			break;

		default:
			colors_selections = MENU_ON;
			colors_enable_menu_timer = TT_MENU_ENABLED;
			break;
	}

	//salgo solo si estoy con el menu prendido
	if ((CheckSSel() > S_HALF) && (colors_selections != MENU_OFF))
	{
		resp = RESP_CHANGE_ALL_UP;
	}


	return resp;
}

unsigned char MenuColors(void)
{
	unsigned char resp = RESP_CONTINUE;
	unsigned char resp_down = RESP_CONTINUE;
	//unsigned char dummy = 0;

	switch (colors_menu_state)
	{
		case COLORS_MENU_INIT:
			//empiezo con las selecciones
			resp_down = FuncShowBlink ((const char *) "Starting Colors ", (const char *) "Selections      ", 1, BLINK_NO);

			if (resp_down == RESP_FINISH)
				colors_menu_state++;
			break;

		case COLORS_MENU_SELECTED:
			if (ConfStruct_local.colors_selected == 0)
				resp_down = 0x80;
			else if (ConfStruct_local.colors_selected == 1)
				resp_down = 0x81;
			else
				resp_down = 0x82;

			FuncOptions ((const char *) "3200K    4500K  ",(const char *) "5600K     back  ", (unsigned char *)s_sel_colors, 4, resp_down);
			colors_menu_state++;
			break;

		case COLORS_MENU_SELECTED_1:

			resp_down = FuncOptions ((const char *) "3200K    4500K  ",(const char *) "5600K     back  ", (unsigned char *)s_sel_colors, 4, 0);

			if ((resp_down & 0x0f) == RESP_SELECTED)
			{
				resp_down = resp_down & 0xf0;
				resp_down >>= 4;
				if (resp_down == 0)
				{
					ConfStruct_local.colors_selected = 0;
				}

				if (resp_down == 1)
				{
					ConfStruct_local.colors_selected = 1;
				}

				if (resp_down == 2)
				{
					ConfStruct_local.colors_selected = 2;
				}

				if (resp_down == 3)
				{
					resp = RESP_WORKING;
					colors_menu_state++;
					LCD_1ER_RENGLON;
					LCDTransmitStr((const char *) "wait to free    ");
				}
				else
				{
					resp = RESP_FINISH;
					colors_menu_state = COLORS_MENU_INIT;
				}
			}
			break;

		case COLORS_MENU_SELECTED_2:	//esto es back voy al menu principal
			if (CheckSSel() == S_NO)
				colors_menu_state = COLORS_MENU_INIT;

			resp = RESP_CHANGE_ALL_UP;
			break;

		default:
			colors_menu_state = COLORS_MENU_INIT;
			break;
	}
	return resp;
}

void FuncColorsReset (void)
{
	colors_state = COLORS_INIT;
}

void MenuColorsReset(void)
{
	colors_menu_state = COLORS_MENU_INIT;
}


