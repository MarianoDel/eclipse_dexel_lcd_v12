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
#include "flash_program.h"
#include "stm32f0xx.h"

#include <stdio.h>
#include <string.h>


/* Externals variables ---------------------------------------------------------*/
extern volatile unsigned short function_timer;
extern volatile unsigned short function_enable_menu_timer;

extern volatile unsigned short function_save_memory_timer;
extern unsigned char function_save_memory;


#define colors_timer function_timer
#define colors_enable_menu_timer function_enable_menu_timer

#define colors_save_memory_timer function_save_memory_timer
#define colors_save_memory function_save_memory

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
unsigned char FuncColors (unsigned char update_screen)
{
	unsigned char resp = RESP_CONTINUE;
	unsigned char resp_down = RESP_CONTINUE;

	if (update_screen == UPDATE_YES)
	{
		colors_state = COLORS_UPDATE;
	}

	switch (colors_state)
	{
		case COLORS_INIT:
			//apago DMX
			DMX_Disa();
			MenuColorsReset();
			colors_state = COLORS_UPDATE;
			colors_selections = MENU_C_ON;
			RELAY1_OFF;
			RELAY2_OFF;
			break;

		case COLORS_UPDATE:
			if (colors_selections == MENU_C_OFF)
			{
				if (ConfStruct_local.colors_selected == 0)
					colors_state = COLORS_3200;
				else if (ConfStruct_local.colors_selected == 1)
					colors_state = COLORS_4500;
				else
					colors_state = COLORS_5600;

				LCD_1ER_RENGLON;
				LCDTransmitStr((const char *) " DEXEL LIGHTING ");

				if (update_screen != UPDATE_YES)
				{
					//se cambio algo pido que se grabe
					colors_save_memory_timer = TT_SAVE_MEMORY;
					colors_save_memory = 1;
				}
				else
				{
					//es solo un update de screen, probablemente despues de OVERTEMP
					update_screen = UPDATE_NO;
					resp = RESP_UPDATED;
				}
			}
			break;

		case COLORS_3200:
			LCD_2DO_RENGLON;
			LCDTransmitStr((const char *) " Color in 3200K ");
			Update_TIM3_CH1 (255);
			Update_TIM3_CH2 (0);

			RELAY1_ON;
			RELAY2_OFF;

			colors_state = COLORS_WAIT_TO_GO;
			break;

		case COLORS_4500:
			LCD_2DO_RENGLON;
			LCDTransmitStr((const char *) " Color in 4500K ");
			Update_TIM3_CH1 (127);
			Update_TIM3_CH2 (127);

			RELAY1_ON;
			RELAY2_ON;

			colors_state = COLORS_WAIT_TO_GO;
			break;

		case COLORS_5600:
			LCD_2DO_RENGLON;
			LCDTransmitStr((const char *) " Color in 5600K ");
			Update_TIM3_CH1 (0);
			Update_TIM3_CH2 (255);

			RELAY1_OFF;
			RELAY2_ON;

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
				colors_selections = MENU_C_ON;
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
		case MENU_C_ON:

			resp_down = MenuColors();

			if (resp_down == RESP_WORKING)	//alguien esta tratando de seleccionar algo, le doy tiempo
			{
				colors_enable_menu_timer = TT_MENU_ENABLED;
				lcd_backlight_timer = TT_LCD_BACKLIGHT;
			}

			if (!colors_enable_menu_timer)	//ya mostre el menu mucho tiempo, lo apago
			{
				LCD_1ER_RENGLON;
				LCDTransmitStr((const char *)s_blank_line);
				LCD_2DO_RENGLON;
				LCDTransmitStr((const char *)s_blank_line);
				colors_selections = MENU_C_OFF;
			}

			if (resp_down == RESP_FINISH)	//se terminaron las selecciones
			{
				colors_enable_menu_timer = 0;
				LCD_1ER_RENGLON;
				LCDTransmitStr((const char *)s_blank_line);
				LCD_2DO_RENGLON;
				LCDTransmitStr((const char *)s_blank_line);

				lcd_backlight_timer = TT_LCD_BACKLIGHT;
				colors_selections = MENU_C_OFF;
			}

			if (resp_down == RESP_CHANGE_ALL_UP)	//se terminaron las selecciones
				resp = RESP_CHANGE_ALL_UP;

			break;

		case MENU_C_OFF:
			//si alguien toco un control prendo el lcd_backlight
			if ((CheckSUp() > S_NO) || (CheckSDown() > S_NO) || (CheckSSel() > S_NO))
				lcd_backlight_timer = TT_LCD_BACKLIGHT;

			break;

		default:
			colors_selections = MENU_C_ON;
			colors_enable_menu_timer = TT_MENU_ENABLED;
			break;
	}

	//salgo solo si estoy con el menu prendido
	if (CheckSSel() > S_HALF)
	{
		FuncColorsReset();
		resp = RESP_CHANGE_ALL_UP;
	}

	//me fijo si necesito grabar si agoto el timer
	if (!colors_save_memory_timer)
	{
		if (colors_save_memory)	//y necesito grabar
		{
			LED_ON;
			colors_save_memory = 0;
			WriteConfigurations();
			LED_OFF;
		}
	}

	return resp;
}

unsigned char MenuColors(void)
{
	unsigned char resp = RESP_CONTINUE;
	unsigned char resp_down = RESP_CONTINUE;

	switch (colors_menu_state)
	{
		case COLORS_MENU_INIT:
			//empiezo con las selecciones
			colors_menu_state++;
			break;

		case COLORS_MENU_SELECTED_0:
			resp_down = FuncShowBlink ((const char *) "Starting Colors ", (const char *) "   Selections   ", 1, BLINK_NO);

			if ((resp_down == RESP_FINISH) && (CheckSUp() == S_NO) && (CheckSDown() == S_NO) && (CheckSSel() == S_NO))
				colors_menu_state++;

			break;

		case COLORS_MENU_SELECTED_1:
			FuncChangeColorsReset ();
			colors_menu_state++;
			break;

		case COLORS_MENU_SELECTED_2:
			resp_down = FuncChangeColors ((unsigned char *) &ConfStruct_local.colors_selected);

			if (resp_down == RESP_FINISH)
			{
				colors_menu_state = COLORS_MENU_INIT;
				resp = RESP_FINISH;
			}
			else if (resp_down == RESP_WORKING)
				resp = RESP_WORKING;

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


