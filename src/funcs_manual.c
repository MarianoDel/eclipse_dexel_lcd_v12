/*
 * funcs_manual.c
 *
 *  Created on: 27/04/2016
 *      Author: Mariano
 */


#include "funcs_manual.h"
#include "hard.h"
#include "main_menu.h"
#include "dmx_transceiver.h"
#include "lcd.h"


/* Externals variables ---------------------------------------------------------*/
extern volatile unsigned short function_timer;
extern volatile unsigned short function_enable_menu_timer;

#define manual_timer function_timer
#define manual_enable_menu_timer function_enable_menu_timer

extern volatile unsigned short lcd_backlight_timer;

extern const char s_blank_line [];

/* Global variables ------------------------------------------------------------*/
enum var_manual_states manual_state = MANUAL_INIT;
unsigned char manual_selections = 0;
unsigned char manual_menu_state = 0;
unsigned short manual_ii = 0;


//-------- Functions -------------

//muestro el menu y hago funciones (pero sin mostrar nada) hasta que pasen 30 segs de TT_MENU_ENABLED
//con func_menu_show
unsigned char FuncManual (void)
{
	unsigned char resp = RESP_CONTINUE;
	unsigned char resp_down = RESP_CONTINUE;

	switch (manual_state)
	{
		case MANUAL_INIT:
			//apago DMX
			DMX_Disa();
			MenuManualReset();
			manual_state = MANUAL_CH1;
			break;

		case MANUAL_UPDATE:		//es un paso adelante, toda la info ya esta en memoria updated
			manual_state++;
			break;

		case MANUAL_CH1:
			//apago DMX
			DMX_Disa();
			MenuManualReset();
			break;

		case MANUAL_CH2:
			//apago DMX
			DMX_Disa();
			MenuManualReset();
			break;

		default:
			manual_state = MANUAL_INIT;
			break;
	}

	//veo el de configuracion hasta TT_MENU_ENABLED
	switch (manual_selections)
	{
		case MENU_ON:

			resp_down = MenuManual();

			if (resp_down == RESP_WORKING)	//alguien esta tratando de seleccionar algo, le doy tiempo
				manual_enable_menu_timer = TT_MENU_ENABLED;

			if (resp_down == RESP_SELECTED)	//se selecciono algo
			{
				manual_enable_menu_timer = TT_MENU_ENABLED;
				manual_selections = MENU_SELECTED;
			}

			if (!manual_enable_menu_timer)	//ya mostre el menu mucho tiempo, lo apago
			{
				LCD_1ER_RENGLON;
				LCDTransmitStr((const char *)s_blank_line);
				LCD_2DO_RENGLON;
				LCDTransmitStr((const char *)s_blank_line);
				manual_selections = MENU_OFF;
			}
			break;

		case MENU_SELECTED:
			//estado algo seleccionado espero update
			resp_down = FuncShowBlink ((const char *) "Something Select", (const char *) "Updating Values", 1, BLINK_NO);

			if (resp_down == RESP_FINISH)
			{
				manual_state = MANUAL_UPDATE;
				manual_selections = MENU_ON;
			}
			break;

		case MENU_OFF:
			//estado menu apagado ESTADO NORMAL
			if (CheckSSel() > S_NO)
			{
				manual_enable_menu_timer = TT_MENU_ENABLED;			//vuelvo a mostrar
				LCD_1ER_RENGLON;
				LCDTransmitStr((const char *) "wait to free    ");
				manual_selections++;
			}
			break;

		case MENU_WAIT_FREE:
			if (CheckSSel() == S_NO)
			{
				manual_selections = MENU_ON;
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

			if (CheckSSel() > S_HALF)
			{
				resp = RESP_CHANGE_ALL_UP;
			}
			break;

		default:
			manual_selections = MENU_ON;
			manual_enable_menu_timer = TT_MENU_ENABLED;
			break;
	}

	return resp;
}

unsigned char MenuManual(void)
{
	unsigned char resp = RESP_CONTINUE;
	unsigned char resp_down = RESP_CONTINUE;
	//unsigned char dummy = 0;

	switch (manual_menu_state)
	{
		case MANUAL_MENU_INIT:
			//empiezo con las selecciones
			resp_down = FuncShowBlink ((const char *) "Starting Manual ", (const char *) "Selections      ", 1, BLINK_NO);

			if (resp_down == RESP_FINISH)
				manual_menu_state++;
			break;

		case MANUAL_MENU_CHANNELS:
			resp_down = FuncShowSelectv2 ((const char * ) "Conf. Channels  ");

			if (resp_down == RESP_CHANGE_UP)	//cambio de menu
				manual_menu_state = MANUAL_MENU_RELAYS;

			if (resp_down == RESP_CHANGE_DWN)	//cambio de menu
				manual_menu_state = MANUAL_MENU_FINISH;

			if (resp_down == RESP_SELECTED)	//se eligio el menu
				manual_menu_state = MANUAL_MENU_CHANNELS_SELECTED;

			if (resp_down != RESP_CONTINUE)
				resp = RESP_WORKING;

			break;

		case MANUAL_MENU_RELAYS:
			resp_down = FuncShowSelectv2 ((const char * ) "Energy Harv Conf");

			if (resp_down == RESP_CHANGE_UP)	//cambio de menu
				manual_menu_state = MANUAL_MENU_FINISH;

			if (resp_down == RESP_CHANGE_DWN)	//cambio de menu
				manual_menu_state = MANUAL_MENU_CHANNELS;

			if (resp_down == RESP_SELECTED)	//se eligio el menu
				manual_menu_state = MANUAL_MENU_RELAYS_SELECTED;

			if (resp_down != RESP_CONTINUE)
				resp = RESP_WORKING;

			break;

		case MANUAL_MENU_FINISH:
			resp_down = FuncShowSelectv2 ((const char * ) "Max Dimming Conf");

			if (resp_down == RESP_CHANGE_UP)	//cambio de menu
				manual_menu_state = MANUAL_MENU_CHANNELS;

			if (resp_down == RESP_CHANGE_DWN)	//cambio de menu
				manual_menu_state = MANUAL_MENU_RELAYS;

			if (resp_down == RESP_SELECTED)	//se eligio el menu
				manual_menu_state = MANUAL_MENU_FINISH_SELECTED;

			if (resp_down != RESP_CONTINUE)
				resp = RESP_WORKING;

			break;

		default:
			manual_menu_state = MANUAL_MENU_INIT;
			break;
	}
	return resp;
}

void FuncManualReset (void)
{
	manual_state = MANUAL_INIT;
}

void MenuManualReset(void)
{
	manual_menu_state = MANUAL_MENU_INIT;
}

