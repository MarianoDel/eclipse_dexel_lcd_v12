/*
 * funcs_dmx.c
 *
 *  Created on: 28/04/2016
 *      Author: Mariano
 */

#include "funcs_manual.h"
#include "hard.h"
#include "main_menu.h"
#include "dmx_transceiver.h"
#include "lcd.h"

#include <stdio.h>
#include <string.h>


/* Externals variables ---------------------------------------------------------*/
extern volatile unsigned short function_timer;
extern volatile unsigned short function_enable_menu_timer;

#define dmx_timer function_timer
#define dmx_enable_menu_timer function_enable_menu_timer

extern volatile unsigned short lcd_backlight_timer;

extern volatile unsigned char DMX_packet_flag;
extern volatile unsigned short DMX_channel_selected;
extern volatile unsigned char DMX_channel_quantity;

#define SIZEOF_DATA1	512
extern volatile unsigned char data1[];
extern volatile unsigned char data[];

extern const char s_blank_line [];

extern Configuration_Typedef ConfStruct_local;

/* Global variables ------------------------------------------------------------*/
enum var_dmx_states dmx_state = DMX_INIT;
unsigned char dmx_state = 0;
unsigned char dmx_selections = 0;
unsigned short dmx_ii = 0;

unsigned char dmx_menu_state = 0;


#define grouped_dimming_top GroupedStruct_local.max_dimmer_value_dmx

unsigned char networked_dim_last = 0;
unsigned char networked_dim2_last = 0;


//-------- Functions -------------
unsigned char FuncDMX (void)
{
	unsigned char resp = RESP_CONTINUE;
	unsigned char resp_down = RESP_CONTINUE;

	short one_int, one_dec;
	char s_lcd [20];

	switch (dmx_state)
	{
		case DMX_INIT:
			//prendo DMX
			DMX_Ena();
			MenuDMXReset();
			dmx_state = DMX_UPDATE;
			break;

		case DMX_UPDATE:		//es un paso adelante, toda la info ya esta en memoria updated
			if (dmx_selections == MENU_OFF)
				dmx_state++;

			break;

		case NETWORKED_WORKING:
			//me quedo aca hasta que me saquen por menu

			if (DMX_packet_flag)
			{
				//llego un paquete DMX
				DMX_packet_flag = 0;

				//en data[1] tengo la info del ch1
				Update_TIM3_CH1 (data[1]);
				networked_ii = data[1];
			}


			if (networked_ii > 10)
				RELAY_ON;
			else if (networked_ii < 5)
				RELAY_OFF;

			if (networked_selections == MENU_OFF)
			{
				if (networked_dim_last != networked_ii)
				{
					unsigned char i;
					unsigned short ii;
					//char s_lcd [20];

					networked_dim_last = networked_ii;

					ii = networked_dim_last * 100;
					ii = ii / 255;
					if (ii > 100)
						ii = 100;

					LCD_2DO_RENGLON;
					Lcd_SetDDRAM(0x40 + 12);
					sprintf(s_lcd, "%3d", ii);
					LCDTransmitStr(s_lcd);
					LCDTransmitStr("%");

					ii = ii / 10;
					if (networked_dim2_last != ii)
					{
						networked_dim2_last = ii;
						LCD_2DO_RENGLON;
						LCDTransmitStr((const char *) "            ");
						LCD_2DO_RENGLON;
						for (i = 0; i < ii; i++)
						{
							LCDStartTransmit(0xff);
						}
					}
				}
			}
			break;

		default:
			networked_state = NETWORKED_INIT;
			break;
	}

	//veo el menu solo si alguien toca los botones o timeout
	switch (networked_selections)
	{
		case MENU_ON:
			//estado normal
			resp_down = MenuNetworked();

			if (resp_down == RESP_WORKING)	//alguien esta tratando de seleccionar algo, le doy tiempo
				networked_enable_menu_timer = 30000;

			if (resp_down == RESP_SELECTED)	//se selecciono algo
			{
				networked_enable_menu_timer = 30000;
				networked_selections = MENU_SELECTED;
			}

			if (!networked_enable_menu_timer)	//agoto el timer
			{
				LCD_1ER_RENGLON;
				sprintf(s_lcd, "Network Ch: %3d ", NetworkedStruct_local.networked_dmx_channel);
				LCDTransmitStr(s_lcd);


				networked_selections = MENU_OFF;
			}
			break;

		case MENU_SELECTED:
			//estado algo seleccionado espero update
			resp_down = FuncShowBlink ((const char *) "Something Select", (const char *) "Updating Values", 1, BLINK_NO);

			if (resp_down == RESP_FINISH)
			{
				networked_state = NETWORKED_UPDATE;
				networked_selections = 0;
			}
			break;

		case MENU_OFF:
			//estado menu apagado
			if ((CheckS1() > S_NO) || (CheckS2() > S_NO))
			{
				networked_enable_menu_timer = 30000;			//vuelvo a mostrar
				LCD_1ER_RENGLON;
				LCDTransmitStr((const char *) "wait to free    ");
				LCD_2DO_RENGLON;
				LCDTransmitStr((const char *) s_blank_line);
				networked_selections++;
			}
			break;

		case MENU_WAIT_FREE:
			if ((CheckS1() == S_NO) && (CheckS2() == S_NO))
			{
				networked_selections = MENU_ON;
				//limpio Menu
				networked_menu_state = NETWORKED_MENU_INIT;
			}
			break;

		default:
			networked_selections = MENU_ON;
			break;
	}

	if (CheckS1() > S_HALF)
		resp = RESP_CHANGE_ALL_UP;

	UpdateRDMResponder();

	return resp;
}


unsigned char MenuNetworked(void)
{
	unsigned char resp = RESP_CONTINUE;
	unsigned char resp_down = RESP_CONTINUE;
	unsigned char dummy = 0;

	switch (networked_menu_state)
	{
		case NETWORKED_MENU_INIT:
			//empiezo con las selecciones
			resp_down = FuncShowBlink ((const char *) "Starting Network", (const char *) "Selections     ", 1, BLINK_NO);

			if (resp_down == RESP_FINISH)
				networked_menu_state = NETWORKED_MENU_INIT_1;
			break;

		case NETWORKED_MENU_INIT_1:
			resp_down = FuncShowBlink ((const char *) "  Network Mode  ",(const char *) s_blank_line, 1, BLINK_NO);

			if (resp_down == RESP_FINISH)
				networked_menu_state = NETWORKED_MENU_SELECT_CHANNEL;

			break;

		case NETWORKED_MENU_SELECT_CHANNEL:
			FuncChangeChannelsReset ();
			networked_menu_state = NETWORKED_MENU_SELECT_CHANNEL_1;
			break;

		case NETWORKED_MENU_SELECT_CHANNEL_1:
			resp_down = FuncChangeChannels ((unsigned char *) &NetworkedStruct_local.networked_dmx_channel);

			if (resp_down == RESP_FINISH)
			{
				resp = RESP_SELECTED;
				networked_menu_state = NETWORKED_MENU_INIT;
			}
			else if (resp_down == RESP_WORKING)
				resp = RESP_WORKING;

			break;


		default:
			networked_menu_state = NETWORKED_MENU_INIT;
			break;
	}
	return resp;
}

