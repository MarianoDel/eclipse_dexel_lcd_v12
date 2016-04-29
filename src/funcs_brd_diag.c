/*
 * funcs_brd_diag.c
 *
 *  Created on: 29/04/2016
 *      Author: Mariano
 */

#include "funcs_brd_diag.h"
#include "hard.h"
#include "main_menu.h"
#include "dmx_transceiver.h"
#include "lcd.h"
#include "stm32f0xx.h"
#include "adc.h"

#include <stdio.h>
#include <string.h>



/* Externals variables ---------------------------------------------------------*/
extern volatile unsigned short function_timer;
extern volatile unsigned short function_enable_menu_timer;

#define brd_diag_timer function_timer
#define brd_diag_enable_menu_timer function_enable_menu_timer

extern volatile unsigned short lcd_backlight_timer;

extern const char s_blank_line [];

extern Configuration_Typedef ConfStruct_local;

/* Global variables ------------------------------------------------------------*/
enum var_brd_diag_states brd_diag_state = BRD_DIAG_INIT;
unsigned char brd_diag_menu_state = 0;

unsigned short last_meas = 0;

#define K_1TO10		0.0392
#define K_TEMP		0.3925
extern float fcalc;



//-------- Functions -------------


//muestro el menu y hago funciones (pero sin mostrar nada) hasta que pasen 30 segs de TT_MENU_ENABLED
//con func_menu_show
unsigned char FuncBrdDiag (void)
{
	unsigned char resp = RESP_CONTINUE;
//	unsigned char resp_down = RESP_CONTINUE;

	unsigned short current_meas, one_int, one_dec;
	char s_lcd[20];

	switch (brd_diag_state)
	{
		case BRD_DIAG_INIT:
			//apago DMX
			DMX_Disa();
			brd_diag_state++;
			break;

		case BRD_DIAG_LED_TEMP:
			LCD_1ER_RENGLON;
			LCDTransmitStr((const char *) "LED Temperature ");
			LCD_2DO_RENGLON;
			LCDTransmitStr(s_blank_line);

			brd_diag_state++;

			break;

		case BRD_DIAG_LED_TEMP_1:
			current_meas = GetLedTemp();
			if (last_meas != current_meas)
			{
				last_meas = current_meas;

				fcalc = last_meas;
				fcalc = fcalc * K_TEMP;
				one_int = (short) fcalc;
				fcalc = fcalc - one_int;
				fcalc = fcalc * 10;
				one_dec = (short) fcalc;

				sprintf(s_lcd, "%2d.%01d", one_int, one_dec);
				Lcd_SetDDRAM(0x40 + 7);
				LCDTransmitStr(s_lcd);
			}

			break;


		default:
			brd_diag_state = BRD_DIAG_INIT;
			break;
	}


	//salgo solo si estoy con el menu prendido
	if (CheckSSel() > S_HALF)
	{
		resp = RESP_CHANGE_ALL_UP;
	}

	lcd_backlight_timer = TT_LCD_BACKLIGHT;

	return resp;
}

void FuncBrdDiagReset (void)
{
	brd_diag_state = BRD_DIAG_INIT;
}

