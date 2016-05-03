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

#define manual_need_a_change function_need_a_change
#define manual_timer function_timer
#define manual_enable_menu_timer function_enable_menu_timer

#define manual_save_memory_timer function_save_memory_timer
#define manual_save_memory function_save_memory

extern volatile unsigned short lcd_backlight_timer;

extern const char s_blank_line [];

extern Configuration_Typedef ConfStruct_local;

/* Global variables ------------------------------------------------------------*/
enum var_manual_states manual_state = MANUAL_INIT;
unsigned char manual_selections = 0;
unsigned char manual_menu_state = 0;
unsigned short manual_ii = 0;

const unsigned char s_sel [] = { 0x02, 0x08, 0x0f };
const unsigned char s_sel_ch [] = { 0x43, 0x48, 0x4f };

#define K_1TO10		0.0392
#define K_100P		0.3925
extern float fcalc;



//-------- Functions -------------

//muestro el menu y hago funciones (pero sin mostrar nada) hasta que pasen 30 segs de TT_MENU_ENABLED
//con func_menu_show
unsigned char FuncManual (unsigned char update_screen)
{
	unsigned char resp = RESP_CONTINUE;
	unsigned char resp_down = RESP_CONTINUE;

	short one_int, one_dec;
	char s_lcd [20];

	if (update_screen == UPDATE_YES)
	{
		manual_state = MANUAL_UPDATE;
	}

	switch (manual_state)
	{
		case MANUAL_INIT:
			//apago DMX
			DMX_Disa();
			MenuManualReset();
			manual_state = MANUAL_UPDATE;
			manual_selections = MENU_M_ON;
			RELAY1_OFF;
			RELAY2_OFF;
			break;

		case MANUAL_UPDATE:		//es un paso adelante, toda la info ya esta en memoria updated
			if (manual_selections == MENU_M_OFF)
			{
				manual_state++;

				if (ConfStruct_local.manual_ch1_value >= RELAY_START)
					RELAY1_ON;
				else if (ConfStruct_local.manual_ch1_value <= RELAY_STOP)
					RELAY1_OFF;

				Update_TIM3_CH1 (ConfStruct_local.manual_ch1_value);

				if (ConfStruct_local.manual_channels_quantity > 1)
				{
					if (ConfStruct_local.manual_ch2_value >= RELAY_START)
						RELAY2_ON;
					else if (ConfStruct_local.manual_ch2_value <= RELAY_STOP)
						RELAY2_OFF;

					Update_TIM3_CH2 (ConfStruct_local.manual_ch2_value);
				}
				else
				{
					RELAY2_OFF;
					Update_TIM3_CH2 (0);
				}

				if (update_screen != UPDATE_YES)
				{
					//se cambio algo pido que se grabe
					manual_save_memory_timer = TT_SAVE_MEMORY;
					manual_save_memory = 1;
				}
				else
				{
					//es solo un update de screen, probablemente despues de OVERTEMP
					update_screen = UPDATE_NO;
					resp = RESP_UPDATED;
				}
			}

			break;

		case MANUAL_CH1:
			LCD_1ER_RENGLON;
			LCDTransmitStr((const char *) " DEXEL LIGHTING ");
			LCD_2DO_RENGLON;
			LCDTransmitStr((const char *) "Ch: 1       %  M");
			manual_state = MANUAL_CH1_1;
			manual_need_a_change = 1;
			break;

		case MANUAL_CH1_1:
			//check s_down, s_up y s_sel
			if (CheckSDown() > S_NO)
			{
				if ((ConfStruct_local.manual_ch1_value > 0) && (!manual_timer))
				{
					manual_timer = TT_UPDATE_BUTTON;
					ConfStruct_local.manual_ch1_value--;

					Update_TIM3_CH1 (ConfStruct_local.manual_ch1_value);
					manual_need_a_change = 1;
				}
			}

			if (CheckSUp() > S_NO)
			{
				if ((ConfStruct_local.manual_ch1_value < 255) && (!manual_timer))
				{
					manual_timer = TT_UPDATE_BUTTON;
					ConfStruct_local.manual_ch1_value++;

					Update_TIM3_CH1 (ConfStruct_local.manual_ch1_value);
					manual_need_a_change = 1;
				}
			}

			if (manual_need_a_change)
			{
				//se cambio algo pido que se grabe
				manual_save_memory_timer = TT_SAVE_MEMORY;
				manual_save_memory = 1;

				manual_need_a_change = 0;
				fcalc = ConfStruct_local.manual_ch1_value;
				fcalc = fcalc * K_100P;
				one_int = (short) fcalc;
				fcalc = fcalc - one_int;
				fcalc = fcalc * 10;
				one_dec = (short) fcalc;

				sprintf(s_lcd, "%3d.%01d", one_int, one_dec);
				Lcd_SetDDRAM(0x40 + 7);
				LCDTransmitStr(s_lcd);

				if (ConfStruct_local.manual_ch1_value >= RELAY_START)
					RELAY1_ON;
				else if (ConfStruct_local.manual_ch1_value <= RELAY_STOP)
					RELAY1_OFF;

			}


			//cambio a canal 2 solo si esta habilitado
			if (ConfStruct_local.manual_channels_quantity > 1)
			{
				if (CheckSSel() > S_NO)
					manual_state = MANUAL_GOING_CH2;

			}
			else
			{
				if (CheckSSel() > S_HALF)
				{
					LCD_1ER_RENGLON;
					LCDTransmitStr((const char *) " Going Menu UP  ");
					LCD_2DO_RENGLON;
					LCDTransmitStr(s_blank_line);

					manual_state = MANUAL_GOING_UP;
				}
			}

			//si el menu no esta apagado vuelvo
			if (manual_selections != MENU_M_OFF)
				manual_state = MANUAL_UPDATE;

			break;

		case MANUAL_GOING_CH2:
			//cambio a canal 2
			if (CheckSSel() == S_NO)
			{
				manual_state = MANUAL_CH2;
			}

			if (CheckSSel() > S_HALF)
			{
				LCD_1ER_RENGLON;
				LCDTransmitStr((const char *) " Going Menu UP  ");
				LCD_2DO_RENGLON;
				LCDTransmitStr(s_blank_line);

				manual_state = MANUAL_GOING_UP;
			}

			break;

		case MANUAL_CH2:
			LCD_1ER_RENGLON;
			LCDTransmitStr((const char *) " DEXEL LIGHTING ");
			LCD_2DO_RENGLON;
			LCDTransmitStr((const char *) "Ch: 2       %  M");
			manual_state = MANUAL_CH2_1;
			manual_need_a_change = 1;
			break;

		case MANUAL_CH2_1:
			//check s_down, s_up y s_sel
			if (CheckSDown() > S_NO)
			{
				if ((ConfStruct_local.manual_ch2_value > 0) && (!manual_timer))
				{
					manual_timer = TT_UPDATE_BUTTON;
					ConfStruct_local.manual_ch2_value--;

					Update_TIM3_CH2 (ConfStruct_local.manual_ch2_value);
					manual_need_a_change = 1;
				}
			}

			if (CheckSUp() > S_NO)
			{
				if ((ConfStruct_local.manual_ch2_value < 255) && (!manual_timer))
				{
					manual_timer = TT_UPDATE_BUTTON;
					ConfStruct_local.manual_ch2_value++;
					Update_TIM3_CH2 (ConfStruct_local.manual_ch2_value);
					manual_need_a_change = 1;
				}
			}

			if (manual_need_a_change)
			{
				//se cambio algo pido que se grabe
				manual_save_memory_timer = TT_SAVE_MEMORY;
				manual_save_memory = 1;

				manual_need_a_change = 0;
				fcalc = ConfStruct_local.manual_ch2_value;
				fcalc = fcalc * K_100P;
				one_int = (short) fcalc;
				fcalc = fcalc - one_int;
				fcalc = fcalc * 10;
				one_dec = (short) fcalc;

				sprintf(s_lcd, "%3d.%01d", one_int, one_dec);
				Lcd_SetDDRAM(0x40 + 7);
				LCDTransmitStr(s_lcd);

				if (ConfStruct_local.manual_ch2_value >= RELAY_START)
					RELAY2_ON;
				else if (ConfStruct_local.manual_ch2_value <= RELAY_STOP)
					RELAY2_OFF;

			}

			if (CheckSSel() > S_NO)
			{
				manual_state = MANUAL_GOING_CH1;
			}

			//si el menu no esta apagado vuelvo
			if (manual_selections != MENU_M_OFF)
				manual_state = MANUAL_UPDATE;

			break;

		case MANUAL_GOING_CH1:
			//estoy en CH2 intento ir a CH1 o going up
			if (CheckSSel() == S_NO)
			{
				manual_state = MANUAL_CH1;
			}

			if (CheckSSel() > S_HALF)
			{
				LCD_1ER_RENGLON;
				LCDTransmitStr((const char *) " Going Menu UP  ");
				LCD_2DO_RENGLON;
				LCDTransmitStr(s_blank_line);

				manual_state = MANUAL_GOING_UP;
			}
			break;

		case MANUAL_GOING_UP:
			//Subo un piso el MENU
			if (CheckSSel() == S_NO)
			{
				MenuManualReset();
				manual_selections = MENU_M_ON;
				manual_state = MANUAL_UPDATE;
				manual_enable_menu_timer = TT_MENU_ENABLED;
			}
			break;


		default:
			manual_state = MANUAL_INIT;
			break;
	}

	//veo el de configuracion hasta TT_MENU_ENABLED
	switch (manual_selections)
	{
		case MENU_M_ON:

			resp_down = MenuManual();

			if (resp_down == RESP_WORKING)	//alguien esta tratando de seleccionar algo, le doy tiempo
			{
				manual_enable_menu_timer = TT_MENU_ENABLED;
				lcd_backlight_timer = TT_LCD_BACKLIGHT;
			}

			if (!manual_enable_menu_timer)	//ya mostre el menu mucho tiempo, lo apago
			{
				LCD_1ER_RENGLON;
				LCDTransmitStr((const char *)s_blank_line);
				LCD_2DO_RENGLON;
				LCDTransmitStr((const char *)s_blank_line);
				manual_selections = MENU_M_OFF;
			}

			if (resp_down == RESP_FINISH)	//se terminaron las selecciones
			{
				manual_enable_menu_timer = 0;
				LCD_1ER_RENGLON;
				LCDTransmitStr((const char *)s_blank_line);
				LCD_2DO_RENGLON;
				LCDTransmitStr((const char *)s_blank_line);

				lcd_backlight_timer = TT_LCD_BACKLIGHT;
				manual_selections = MENU_M_OFF;
			}
			break;

		case MENU_M_OFF:
			//si alguien toco un control prendo el lcd_backlight
			if ((CheckSUp() > S_NO) || (CheckSDown() > S_NO) || (CheckSSel() > S_NO))
				lcd_backlight_timer = TT_LCD_BACKLIGHT;

			break;

		default:
			manual_selections = MENU_M_ON;
			manual_enable_menu_timer = TT_MENU_ENABLED;
			break;
	}

	//salgo solo si estoy con el menu prendido
	if (CheckSSel() > S_HALF)
	{
		FuncManualReset();
		resp = RESP_CHANGE_ALL_UP;
	}

	//me fijo si necesito grabar si agoto el timer
	if (!manual_save_memory_timer)
	{
		if (manual_save_memory)	//y necesito grabar
		{
			LED_ON;
			manual_save_memory = 0;
			WriteConfigurations();
			LED_OFF;
		}
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
			manual_menu_state++;
			break;

		case MANUAL_MENU_CHANNELS_0:
			resp_down = FuncShowBlink ((const char *) " Select Manual  ", (const char *) "    Channels    ", 1, BLINK_NO);

			if ((resp_down == RESP_FINISH) && (CheckSUp() == S_NO) && (CheckSDown() == S_NO) && (CheckSSel() == S_NO))
				manual_menu_state++;

			break;

		case MANUAL_MENU_CHANNELS_1:
			FuncChangeChannelsReset ();
			manual_menu_state++;
			break;

		case MANUAL_MENU_CHANNELS_2:
			resp_down = FuncChangeChannels ((unsigned char *) &ConfStruct_local.manual_channels_quantity);

			if (resp_down == RESP_FINISH)
			{
				manual_menu_state = MANUAL_MENU_INIT;
				resp = RESP_FINISH;
			}
			else if (resp_down == RESP_WORKING)
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

