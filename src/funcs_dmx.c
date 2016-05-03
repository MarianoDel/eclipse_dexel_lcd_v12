/*
 * funcs_dmx.c
 *
 *  Created on: 28/04/2016
 *      Author: Mariano
 */

#include "funcs_dmx.h"
#include "hard.h"
#include "main_menu.h"
#include "dmx_transceiver.h"
#include "lcd.h"
#include "stm32f0xx.h"
#include "flash_program.h"

#include <stdio.h>
#include <string.h>


/* Externals variables ---------------------------------------------------------*/
extern volatile unsigned short function_timer;
extern volatile unsigned short function_enable_menu_timer;
extern unsigned char function_need_a_change;

extern volatile unsigned short function_save_memory_timer;
extern unsigned char function_save_memory;


#define dmx_need_a_change function_need_a_change
#define dmx_timer function_timer
#define dmx_enable_menu_timer function_enable_menu_timer

#define dmx_save_memory_timer function_save_memory_timer
#define dmx_save_memory function_save_memory


extern volatile unsigned short lcd_backlight_timer;

extern volatile unsigned char DMX_packet_flag;
extern volatile unsigned short DMX_channel_selected;
extern volatile unsigned char DMX_channel_quantity;

#define SIZEOF_DATA1	512
extern volatile unsigned char data1[];
extern volatile unsigned char data[];

extern unsigned char vd0 [];
extern unsigned char vd1 [];
extern unsigned char MAFilter (unsigned char, unsigned char *);

extern volatile unsigned char filter_timer;

extern const char s_blank_line [];

extern Configuration_Typedef ConfStruct_local;

/* Global variables ------------------------------------------------------------*/
enum var_dmx_states dmx_state = DMX_INIT;
unsigned char dmx_selections = 0;
unsigned short dmx_ii = 0;

unsigned char dmx_menu_state = 0;


#define grouped_dimming_top GroupedStruct_local.max_dimmer_value_dmx

unsigned char networked_dim_last = 0;
unsigned char networked_dim2_last = 0;

const unsigned char s_sel_dmx [] = { 0x02, 0x08, 0x0f };
const unsigned char s_sel_ch_dmx [] = { 0x43, 0x48, 0x4f };

#define K_1TO10		0.0392
#define K_100P		0.3925
extern float fcalc;

unsigned char last_ch1;
unsigned char last_ch2;

//-------- Functions -------------
unsigned char FuncDMX (unsigned char update_screen)
{
	unsigned char resp = RESP_CONTINUE;
	unsigned char resp_down = RESP_CONTINUE;

	short one_int, one_dec;
	char s_lcd [20];

	if (update_screen == UPDATE_YES)
	{
		dmx_state = DMX_UPDATE;
	}


	switch (dmx_state)
	{
		case DMX_INIT:
			//prendo DMX
			DMX_Ena();
			MenuDMXReset();
			dmx_state = DMX_UPDATE;
			dmx_selections = MENU_D_ON;
			RELAY1_OFF;
			RELAY2_OFF;
			break;

		case DMX_UPDATE:		//es un paso adelante, toda la info ya esta en memoria updated
			if (dmx_selections == MENU_D_OFF)
			{
				dmx_state++;

				DMX_channel_selected = ConfStruct_local.dmx_addr;
				DMX_channel_quantity = ConfStruct_local.dmx_channel_quantity;

				if (data[1] >= RELAY_START)
					RELAY1_ON;
				else if (data[1] <= RELAY_STOP)
					RELAY1_OFF;

				if (ConfStruct_local.dmx_channel_quantity > 1)
				{
					if (data[2] >= RELAY_START)
						RELAY2_ON;
					else if (data[2] <= RELAY_STOP)
						RELAY2_OFF;
				}
				else
					RELAY2_OFF;

				last_ch1 = 0;
				last_ch2 = 0;

				if (update_screen != UPDATE_YES)
				{
					//se cambio algo pido que se grabe
					dmx_save_memory_timer = TT_SAVE_MEMORY;
					dmx_save_memory = 1;
				}
				else
				{
					//es solo un update de screen, probablemente despues de OVERTEMP
					update_screen = UPDATE_NO;
					resp = RESP_UPDATED;
				}
			}
			break;

		case DMX_CH1:
			LCD_1ER_RENGLON;
			LCDTransmitStr((const char *) " DEXEL LIGHTING ");
			LCD_2DO_RENGLON;
			LCDTransmitStr((const char *) "Ch: 1       %  D");
			dmx_state = DMX_CH1_1;
			dmx_need_a_change = 1;
			break;

		case DMX_CH1_1:

			//me fijo si tengo nuevo paquete DMX
			if (DMX_packet_flag)
			{
				 DMX_packet_flag = 0;

				 lcd_backlight_timer = TT_LCD_BACKLIGHT;

				 //en data tengo la info CH1 funciona siempre CH2 reviso si debe funcionar
				 if (last_ch1 != data[1])
				 {
					 last_ch1 = data[1];
					 dmx_need_a_change = 1;
				 }
				 else if ((ConfStruct_local.dmx_channel_quantity > 1) && (last_ch2 != data[2]))
				 {
					 last_ch2 = data[2];
					 dmx_state = DMX_CH2;
				 }
			}

			if (dmx_need_a_change)
			{
				dmx_need_a_change = 0;
				fcalc = last_ch1;
				fcalc = fcalc * K_100P;
				one_int = (short) fcalc;
				fcalc = fcalc - one_int;
				fcalc = fcalc * 10;
				one_dec = (short) fcalc;

				sprintf(s_lcd, "%3d.%01d", one_int, one_dec);
				Lcd_SetDDRAM(0x40 + 7);
				LCDTransmitStr(s_lcd);

				if (last_ch1 >= RELAY_START)
					RELAY1_ON;
				else if (last_ch1 <= RELAY_STOP)
					RELAY1_OFF;

			}


			//cambio a canal 2 solo si esta habilitado
			if (ConfStruct_local.dmx_channel_quantity > 1)
			{
				if (CheckSSel() > S_NO)
					dmx_state = DMX_GOING_CH2;

			}
			else
			{
				if (CheckSSel() > S_HALF)
				{
					LCD_1ER_RENGLON;
					LCDTransmitStr((const char *) " Going Menu UP  ");
					LCD_2DO_RENGLON;
					LCDTransmitStr(s_blank_line);

					dmx_state = DMX_GOING_UP;
				}
			}

			//si el menu no esta apagado vuelvo
			if (dmx_selections != MENU_D_OFF)
				dmx_state = DMX_UPDATE;

			break;

		case DMX_GOING_CH2:
			//cambio a canal 2
			if (CheckSSel() == S_NO)
			{
				dmx_state = DMX_CH2;
			}

			if (CheckSSel() > S_HALF)
			{
				LCD_1ER_RENGLON;
				LCDTransmitStr((const char *) " Going Menu UP  ");
				LCD_2DO_RENGLON;
				LCDTransmitStr(s_blank_line);

				dmx_state = DMX_GOING_UP;
			}

			break;

		case DMX_CH2:
			LCD_1ER_RENGLON;
			LCDTransmitStr((const char *) " DEXEL LIGHTING ");
			LCD_2DO_RENGLON;
			LCDTransmitStr((const char *) "Ch: 2       %  D");
			dmx_state = DMX_CH2_1;
			dmx_need_a_change = 1;
			break;

		case DMX_CH2_1:
			//check s_down, s_up y s_sel
			//me fijo si tengo nuevo paquete DMX
			if (DMX_packet_flag)
			{
				 DMX_packet_flag = 0;

				 lcd_backlight_timer = TT_LCD_BACKLIGHT;
				 //en data tengo la info
				 if (last_ch2 != data[2])
				 {
					 last_ch2 = data[2];
					 dmx_need_a_change = 1;
					 //TODO: cargar al filtro
				 }
				 else if (last_ch1 != data[1])
				 {
					 last_ch1 = data[1];
					 dmx_state = DMX_CH1;
					 //TODO: cargar al filtro
				 }
			}

			if (dmx_need_a_change)
			{
				dmx_need_a_change = 0;
				fcalc = last_ch2;
				fcalc = fcalc * K_100P;
				one_int = (short) fcalc;
				fcalc = fcalc - one_int;
				fcalc = fcalc * 10;
				one_dec = (short) fcalc;

				sprintf(s_lcd, "%3d.%01d", one_int, one_dec);
				Lcd_SetDDRAM(0x40 + 7);
				LCDTransmitStr(s_lcd);

				if (last_ch2 >= RELAY_START)
					RELAY2_ON;
				else if (last_ch2 <= RELAY_STOP)
					RELAY2_OFF;

			}

			if (CheckSSel() > S_NO)
			{
				dmx_state = DMX_GOING_CH1;
			}

			//si el menu no esta apagado vuelvo
			if (dmx_selections != MENU_D_OFF)
				dmx_state = DMX_UPDATE;

			break;

		case DMX_GOING_CH1:
			//estoy en CH2 intento ir a CH1 o going up
			if (CheckSSel() == S_NO)
			{
				dmx_state = DMX_CH1;
			}

			if (CheckSSel() > S_HALF)
			{
				LCD_1ER_RENGLON;
				LCDTransmitStr((const char *) " Going Menu UP  ");
				LCD_2DO_RENGLON;
				LCDTransmitStr(s_blank_line);

				dmx_state = DMX_GOING_UP;
			}
			break;

		case DMX_GOING_UP:
			//Subo un piso el MENU
			if (CheckSSel() == S_NO)
			{
				MenuDMXReset();
				dmx_selections = MENU_D_ON;
				dmx_state = DMX_UPDATE;
				dmx_enable_menu_timer = TT_MENU_ENABLED;
			}
			break;


		default:
			dmx_state = DMX_INIT;
			break;
	}

	if (!filter_timer)
	{
		filter_timer = 5;

		Update_TIM3_CH1 (MAFilter(data[1], vd0));
		Update_TIM3_CH2 (MAFilter(data[2], vd1));
	}

	//veo el de configuracion hasta TT_MENU_ENABLED
	switch (dmx_selections)
	{
		case MENU_D_ON:

			resp_down = MenuDMX();

			if (resp_down == RESP_WORKING)	//alguien esta tratando de seleccionar algo, le doy tiempo
			{
				dmx_enable_menu_timer = TT_MENU_ENABLED;
				lcd_backlight_timer = TT_LCD_BACKLIGHT;
			}

			if (!dmx_enable_menu_timer)	//ya mostre el menu mucho tiempo, lo apago
			{
				LCD_1ER_RENGLON;
				LCDTransmitStr((const char *)s_blank_line);
				LCD_2DO_RENGLON;
				LCDTransmitStr((const char *)s_blank_line);
				dmx_selections = MENU_D_OFF;
			}

			if (resp_down == RESP_FINISH)	//se terminaron las selecciones
			{
				dmx_enable_menu_timer = 0;
				LCD_1ER_RENGLON;
				LCDTransmitStr((const char *)s_blank_line);
				LCD_2DO_RENGLON;
				LCDTransmitStr((const char *)s_blank_line);

				lcd_backlight_timer = TT_LCD_BACKLIGHT;
				dmx_selections = MENU_D_OFF;
			}
			break;

		case MENU_D_OFF:
			//si alguien toco un control prendo el lcd_backlight
			if ((CheckSUp() > S_NO) || (CheckSDown() > S_NO) || (CheckSSel() > S_NO))
				lcd_backlight_timer = TT_LCD_BACKLIGHT;

			break;

		default:
			dmx_selections = MENU_D_ON;
			dmx_enable_menu_timer = TT_MENU_ENABLED;
			break;
	}


	//salgo del menu si no estoy eligiendo address del DMX
	if (CheckSSel() > S_HALF)
	{
		FuncDMXReset();
		resp = RESP_CHANGE_ALL_UP;
	}


	//me fijo si necesito grabar si agoto el timer
	if (!dmx_save_memory_timer)
	{
		if (dmx_save_memory)	//y necesito grabar
		{
			LED_ON;
			DMX_Disa();
			dmx_save_memory = 0;
			WriteConfigurations();
			DMX_Ena();
			LED_OFF;
		}
	}


	return resp;
}

unsigned char MenuDMX(void)
{
	unsigned char resp = RESP_CONTINUE;
	unsigned char resp_down = RESP_CONTINUE;
	//unsigned char dummy = 0;

	switch (dmx_menu_state)
	{
		case DMX_MENU_INIT:
			//empiezo con las selecciones
			dmx_menu_state++;
			break;

		case DMX_MENU_ADDRESS_0:
			resp_down = FuncShowBlink ((const char *) "   Select DMX   ", (const char *) "    Address     ", 1, BLINK_NO);

			if ((resp_down == RESP_FINISH) && (CheckSUp() == S_NO) && (CheckSDown() == S_NO) && (CheckSSel() == S_NO))
				dmx_menu_state++;
			break;

		case DMX_MENU_ADDRESS_1:
			FuncChangeAddressReset ();
			dmx_menu_state++;
			break;

		case DMX_MENU_ADDRESS_2:
			resp_down = FuncChangeAddress ((unsigned short *) &ConfStruct_local.dmx_addr);

			if (resp_down == RESP_FINISH)
			{
				resp = RESP_WORKING;
				dmx_menu_state = DMX_MENU_CHANNELS_0;
			}
			else if (resp_down == RESP_WORKING)
				resp = RESP_WORKING;

			break;

		case DMX_MENU_CHANNELS_0:
			resp_down = FuncShowBlink ((const char *) "   Select DMX   ", (const char *) "    Channels    ", 1, BLINK_NO);

			if ((resp_down == RESP_FINISH) && (CheckSUp() == S_NO) && (CheckSDown() == S_NO) && (CheckSSel() == S_NO))
				dmx_menu_state++;
			break;

		case DMX_MENU_CHANNELS_1:
			FuncChangeChannelsReset ();
			dmx_menu_state++;
			break;

		case DMX_MENU_CHANNELS_2:
			resp_down = FuncChangeChannels ((unsigned char *) &ConfStruct_local.dmx_channel_quantity);

			if (resp_down == RESP_FINISH)
			{
				dmx_menu_state = DMX_MENU_INIT;
				resp = RESP_FINISH;
			}
			else if (resp_down == RESP_WORKING)
				resp = RESP_WORKING;

			break;

		default:
			dmx_menu_state = DMX_MENU_INIT;
			break;
	}
	return resp;
}

void FuncDMXReset (void)
{
	dmx_state = DMX_INIT;
}

void MenuDMXReset(void)
{
	dmx_menu_state = DMX_MENU_INIT;
}

