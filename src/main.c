/**
  ******************************************************************************
  * @file    Template_2/main.c
  * @author  Nahuel
  * @version V1.0
  * @date    22-August-2014
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * Use this template for new projects with stm32f0xx family.
  *
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"
#include "stm32f0xx_conf.h"
#include "stm32f0xx_adc.h"
//#include "stm32f0xx_can.h"
//#include "stm32f0xx_cec.h"
//#include "stm32f0xx_comp.h"
//#include "stm32f0xx_crc.h"
//#include "stm32f0xx_crs.h"
//#include "stm32f0xx_dac.h"
//#include "stm32f0xx_dbgmcu.h"
//#include "stm32f0xx_dma.h"
//#include "stm32f0xx_exti.h"
//#include "stm32f0xx_flash.h"
#include "stm32f0xx_gpio.h"
//#include "stm32f0xx_i2c.h"
//#include "stm32f0xx_iwdg.h"
#include "stm32f0xx_misc.h"
//#include "stm32f0xx_pwr.h"
#include "stm32f0xx_rcc.h"
//#include "stm32f0xx_rtc.h"
#include "stm32f0xx_spi.h"
//#include "stm32f0xx_syscfg.h"
#include "stm32f0xx_tim.h"
#include "stm32f0xx_usart.h"
//#include "stm32f0xx_wwdg.h"
#include "system_stm32f0xx.h"
#include "stm32f0xx_it.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


//--- My includes ---//
#include "stm32f0x_gpio.h"
#include "stm32f0x_tim.h"
#include "stm32f0x_uart.h"

#include "hard.h"
//#include "main.h"

#include "lcd.h"

#include "core_cm0.h"
#include "adc.h"
#include "flash_program.h"
#include "main_menu.h"
#include "synchro.h"
#include "dmx_transceiver.h"
#include "funcs_manual.h"
#include "funcs_colors.h"
#include "funcs_dmx.h"
#include "funcs_brd_diag.h"


//--- VARIABLES EXTERNAS ---//
volatile unsigned char timer_1seg = 0;

//volatile unsigned short timer_led_comm = 0;
//volatile unsigned short timer_for_cat_switch = 0;
//volatile unsigned short timer_for_cat_display = 0;
volatile unsigned char buffrx_ready = 0;
volatile unsigned char *pbuffrx;
volatile unsigned short wait_ms_var = 0;

//volatile unsigned char TxBuffer_SPI [TXBUFFERSIZE];
//volatile unsigned char RxBuffer_SPI [RXBUFFERSIZE];
//volatile unsigned char *pspi_tx;
//volatile unsigned char *pspi_rx;
//volatile unsigned char spi_bytes_left = 0;

// ------- Externals Estructura de parametros generales -------
Configuration_Typedef ConfStruct_local;

// ------- Externals del DMX -------
volatile unsigned char Packet_Detected_Flag;
volatile unsigned char DMX_packet_flag;
volatile unsigned char RDM_packet_flag;
volatile unsigned char dmx_receive_flag = 0;
volatile unsigned short DMX_channel_received = 0;
volatile unsigned short DMX_channel_selected = 1;
volatile unsigned char DMX_channel_quantity = 4;

volatile unsigned char data1[512];
//static unsigned char data_back[10];
volatile unsigned char data[12];

// ------- Externals de los timers -------
//volatile unsigned short prog_timer = 0;
//volatile unsigned short mainmenu_timer = 0;
volatile unsigned short show_select_timer = 0;
volatile unsigned char switches_timer = 0;
volatile unsigned short timer_fan_freerun = 0;

volatile unsigned short scroll1_timer = 0;
volatile unsigned short scroll2_timer = 0;

volatile unsigned short function_timer;
volatile unsigned short function_enable_menu_timer;
unsigned char function_need_a_change = 0;
volatile unsigned short function_save_memory_timer;
unsigned char function_save_memory;

volatile unsigned short lcd_backlight_timer = 0;

//volatile unsigned short standalone_menu_timer;
//volatile unsigned char grouped_master_timeout_timer;
volatile unsigned short take_temp_sample = 0;
volatile unsigned short take_led_temp_sample = 0;
volatile unsigned short minutes = 0;


// ------- del display LCD -------
const char s_blank_line [] = {"                "};


// ------- Externals de los switches -------
unsigned short sdown;
unsigned short sup;
unsigned short ssel;

#define K_1TO10		0.0392
#define K_100P		0.3925
float fcalc = 0.0;


//--- VARIABLES GLOBALES ---//
//parameters_typedef param_struct;

// ------- de los timers -------
volatile unsigned short timer_standby;
//volatile unsigned char display_timer;
volatile unsigned char filter_timer;

//volatile unsigned char door_filter;
//volatile unsigned char take_sample;
//volatile unsigned char move_relay;
volatile unsigned short secs = 0;

// ------- Para configuracion Inicial -------
/*
Configuration_Typedef const ConfStruct_constant =
//StandAlone_Typedef __attribute__ ((section("memParams"))) const StandAloneStruct_constant =
		{
				.move_sensor_enable = 1,
				.ldr_enable = 0,
				.ldr_value = 100,
				.max_dimmer_value_percent = 100,
				.max_dimmer_value_dmx = 255,
				.min_dimmer_value_percent = 1,
				.min_dimmer_value_dmx = MIN_DIMMING,
				.power_up_timer_value = 3000,
				.dimming_up_timer_value = 3000
		};
*/


// ------- del DMX -------
volatile unsigned char signal_state = 0;
volatile unsigned char dmx_timeout_timer = 0;
//unsigned short tim_counter_65ms = 0;

// ------- de los filtros DMX -------
#define LARGO_F		16
#define DIVISOR_F	4
unsigned char vd0 [LARGO_F + 1];
unsigned char vd1 [LARGO_F + 1];


#define IDLE	0
#define LOOK_FOR_BREAK	1
#define LOOK_FOR_MARK	2
#define LOOK_FOR_START	3

//--- FUNCIONES DEL MODULO ---//
void TimingDelay_Decrement(void);
void UpdatePackets (void);

// ------- del display -------



// ------- del DMX -------
extern void EXTI4_15_IRQHandler(void);
#define DMX_TIMEOUT	20
unsigned char MAFilter (unsigned char, unsigned char *);

//--- FILTROS DE SENSORES ---//
#define LARGO_FILTRO 16
#define DIVISOR      4   //2 elevado al divisor = largo filtro
//#define LARGO_FILTRO 32
//#define DIVISOR      5   //2 elevado al divisor = largo filtro
unsigned short vtemp [LARGO_FILTRO + 1];
unsigned short vpote [LARGO_FILTRO + 1];

//--- FIN DEFINICIONES DE FILTRO ---//


//-------------------------------------------//
// @brief  Main program.
// @param  None
// @retval None
//------------------------------------------//
int main(void)
{
	unsigned char i;
	unsigned short current_led_temp;
	unsigned char resp = RESP_CONTINUE;
	unsigned char update_s = UPDATE_NO;

	enum var_main_states main_state = MAIN_INIT;
	enum var_main_states last_main_state;


	//!< At this stage the microcontroller clock setting is already configured,
    //   this is done through SystemInit() function which is called from startup
    //   file (startup_stm32f0xx.s) before to branch to application main.
    //   To reconfigure the default setting of SystemInit() function, refer to
    //   system_stm32f0xx.c file

	//GPIO Configuration.
	GPIO_Config();


	//ACTIVAR SYSTICK TIMER
	if (SysTick_Config(48000))
	{
		while (1)	/* Capture error */
		{
			if (LED)
				LED_OFF;
			else
				LED_ON;

			for (i = 0; i < 255; i++)
			{
				asm (	"nop \n\t"
						"nop \n\t"
						"nop \n\t" );
			}
		}
	}

	//ADC Configuration
	AdcConfig();

	//TIM Configuration.
	TIM_3_Init();
	TIM_14_Init();			//lo uso para detectar el break en el DMX
	//TIM_16_Init();		//para OneShoot() cuando funciona en modo master
	//TIM_17_Init();		//lo uso para el ADC de Igrid

	//--- PRUEBA DISPLAY LCD ---
	EXTIOff ();
	LCDInit();
	LED_ON;

	//--- Welcome code ---//
	Lcd_Command(CLEAR);
	Wait_ms(100);
	Lcd_Command(CURSOR_OFF);
	Wait_ms(100);
	Lcd_Command(BLINK_OFF);
	Wait_ms(100);
	CTRL_BKL_ON;

	while (FuncShowBlink ((const char *) "Kirno Technology", (const char *) "Smart Controller", 2, BLINK_NO) != RESP_FINISH);
	LED_OFF;
	while (FuncShowBlink ((const char *) "Hardware: V1.2  ", (const char *) "Software: V2.1  ", 1, BLINK_CROSS) != RESP_FINISH);

	//DE PRODUCCION Y PARA PRUEBAS EN DMX
	Packet_Detected_Flag = 0;
	DMX_channel_selected = 1;
	DMX_channel_quantity = 4;
	USART1Config();


	Update_TIM3_CH2 (255);

	//---------- Prueba Board Temp --------//
	//---------- Prueba LED Temp --------//
	/*
	while (1)
	{
		local_meas = GetBoardTemp();
		if (local_meas != local_meas_last)
		{
			LED_ON;
			local_meas_last = local_meas;
			LCD_2DO_RENGLON;
			LCDTransmitStr((const char *) "Brd Temp:       ");
			local_meas = ConvertBoardTemp(local_meas);
			sprintf(s_lcd, "%d", local_meas);
			Lcd_SetDDRAM(0x40 + 10);
			LCDTransmitStr(s_lcd);
			LED_OFF;
		}

		local_meas = GetLedTemp();
		if (local_meas != local_meas_led_last)
		{
			LED_ON;
			local_meas_led_last = local_meas;
			LCD_1ER_RENGLON;
			LCDTransmitStr((const char *) "Led Temp:       ");
			sprintf(s_lcd, "%d", local_meas);
			Lcd_SetDDRAM(0x00 + 10);
			LCDTransmitStr(s_lcd);
			LED_OFF;
		}

		UpdateBoardTemp();
		UpdateLedTemp();
	}
	*/
	//---------- Fin prueba Board Temp --------//
	//---------- Fin Prueba LED Temp ----------//


	//---------- Prueba FAN --------//
	/*
	while (1)
	{
		if (!timer_standby)
		{
			timer_standby = 10000;
			local_meas++;
		}

		if (local_meas == 1)
		{
			LCD_1ER_RENGLON;
			LCDTransmitStr((const char *) "FAN: Low Speed  ");
			SetPWMFan(FAN_SPEED_LOW);
			local_meas++;
		}

		if (local_meas == 3)
		{
			LCD_1ER_RENGLON;
			LCDTransmitStr((const char *) "FAN: Med Speed  ");

			SetPWMFan(FAN_SPEED_MED);
			local_meas++;
		}

		if (local_meas == 5)
		{
			LCD_1ER_RENGLON;
			LCDTransmitStr((const char *) "FAN: High Speed ");

			SetPWMFan(FAN_SPEED_HIGH);
			local_meas = 0;
		}

		UpdateFan();
	}
	*/
	//---------- Fin prueba FAN --------//


	//---------- Prueba 1 to 10V --------//
	/*
	local_meas = 0;
	while (1)
	{
		LCD_2DO_RENGLON;
		LCDTransmitStr((const char *) "1 to 10V:       ");
		fcalc = local_meas;
		fcalc = fcalc * K_1TO10;
		one_int = (short) fcalc;
		fcalc = fcalc - one_int;
		fcalc = fcalc * 10;
		one_dec = (short) fcalc;

		sprintf(s_lcd, "%02d.%01d V", one_int, one_dec);
		Lcd_SetDDRAM(0x40 + 10);
		LCDTransmitStr(s_lcd);

		Wait_ms (100);
		if (local_meas <= 255)
			local_meas++;
		else
			local_meas = 0;

		Update_TIM3_CH2 (local_meas);

		if (!local_meas)
			Wait_ms(1000);

	}
	*/
	//---------- Fin prueba 1 to 10V --------//

	//---------- Prueba de RELAYS -----------//
	/*
	while (1)
	{
		//resp = FuncStandAloneCert();

		LED_ON;
		RELAY1_ON;
		RELAY2_OFF;
		LCD_2DO_RENGLON;
		LCDTransmitStr((const char *) "RELAY CH1       ");

		Wait_ms(5000);

		LED_OFF;
		RELAY1_OFF;
		RELAY2_ON;
		LCD_2DO_RENGLON;
		LCDTransmitStr((const char *) "RELAY CH2       ");

		Wait_ms(5000);

		UpdateSwitches();
		UpdatePackets();
		UpdateTemp();
		UpdateIGrid();		//OJO que LCD lleva algo de tiempo y quita determinacion

	}	//termina while(1)
	*/
	//---------- Fin prueba RELAYS --------//

	//--- PRUEBA CH0 DMX con switch de display	inicializo mas arriba USART y variables
	/*
	 LCD_1ER_RENGLON;
	 LCDTransmitStr((const char *) "    DMX Test    ");
	 DMX_Ena();

	 while (1)
	 {
		 if (DMX_packet_flag)
		 {
			 //llego un paquete DMX
			 DMX_packet_flag = 0;
			 //en data tengo la info

			 if (local_meas != data[1])
			 {
				 local_meas = data[1];

				 Update_TIM3_CH1 (data[1]);
				 Update_TIM3_CH2 (data[2]);

				 LCD_2DO_RENGLON;
				 LCDTransmitStr((const char *) "DMX Ch1:        ");
				 fcalc = local_meas;
				 fcalc = fcalc * K_100P;
				 one_int = (short) fcalc;
				 fcalc = fcalc - one_int;
				 fcalc = fcalc * 10;
				 one_dec = (short) fcalc;

				 sprintf(s_lcd, "%3d.%01d", one_int, one_dec);
				 Lcd_SetDDRAM(0x40 + 9);
				 LCDTransmitStr(s_lcd);
				 LCDTransmitStr((const char *) "%");

			 }

		 }

		 UpdateSwitches ();
		 UpdatePackets ();

	 }
	*/
	//--- FIN PRUEBA CH0 DMX

	//---------- Comienza Programa de PRODUCCION --------//

	//leo la memoria, si tengo configuracion de modo
	//entro directo, sino a Main Menu

	memcpy(&ConfStruct_local, (Configuration_Typedef *) PAGE31, sizeof(ConfStruct_local));

	while (FuncShowBlink ((const char *) "Checking Memory ", s_blank_line, 1, BLINK_NO) != RESP_FINISH);

	//REVISO SI LA MEMORIA ESTA BORRADA
	if (ConfStruct_local.general_mode == GENERAL_DMX_MODE)
		main_state = MAIN_DMX;
	else if (ConfStruct_local.general_mode == GENERAL_MANUAL_MODE)
		main_state = MAIN_MANUAL;
	else if	(ConfStruct_local.general_mode == GENERAL_COLORS_MODE)
		main_state = MAIN_COLORS;
	else
	{
		//Memoria Borrada
		while (FuncShowBlink ((const char *) "  Memory Empty  ", s_blank_line, 1, BLINK_NO) != RESP_FINISH);
		main_state = MAIN_INIT;

		//default para MANUAL

		//default para DMX
		ConfStruct_local.dmx_addr = 1;

		//default para COLORS

	}
	lcd_backlight_timer = TT_LCD_BACKLIGHT;
	update_s = UPDATE_NO;

	while (1)
	{
		switch (main_state)
		{
			case MAIN_INIT:
				resp = FuncMainMenu();

				RELAY1_OFF;
				RELAY2_OFF;

				if (resp == MAINMENU_SHOW_MANUAL_SELECTED)
				{
					main_state = MAIN_MANUAL;
					ConfStruct_local.general_mode = GENERAL_MANUAL_MODE;
					function_enable_menu_timer = TT_MENU_ENABLED;
				}

				if (resp == MAINMENU_SHOW_DMX_SELECTED)
				{
					main_state = MAIN_DMX;
					ConfStruct_local.general_mode = GENERAL_DMX_MODE;
					function_enable_menu_timer = TT_MENU_ENABLED;
				}

				if (resp == MAINMENU_SHOW_COLORS_SELECTED)
				{
					main_state = MAIN_COLORS;
					ConfStruct_local.general_mode = GENERAL_COLORS_MODE;
					function_enable_menu_timer = TT_MENU_ENABLED;
				}

				if (resp == MAINMENU_SHOW_BRD_DIAG_SELECTED)
					main_state = MAIN_BRD_DIAG;

				break;

			case MAIN_DMX:

				resp = FuncDMX(update_s);

				if (resp == RESP_UPDATED)		//se utiliza para refresco de screen
					update_s = UPDATE_NO;		//generalmente luego de OVERTEMP

				if (resp == RESP_CHANGE_ALL_UP)
				{
					FuncDMXReset();
					main_state = MAIN_INIT;
				}
				break;

			case MAIN_MANUAL:

				resp = FuncManual(update_s);

				if (resp == RESP_UPDATED)		//se utiliza para refresco de screen
					update_s = UPDATE_NO;		//generalmente luego de OVERTEMP

				if (resp == RESP_CHANGE_ALL_UP)
				{
					FuncManualReset();
					main_state = MAIN_INIT;
				}
				break;

			case MAIN_COLORS:

				resp = FuncColors(update_s);

				if (resp == RESP_UPDATED)		//se utiliza para refresco de screen
					update_s = UPDATE_NO;		//generalmente luego de OVERTEMP

				if (resp == RESP_CHANGE_ALL_UP)
				{
					FuncColorsReset();
					main_state = MAIN_INIT;
				}
				break;

			case MAIN_BRD_DIAG:
				resp = FuncBrdDiag();

				RELAY1_OFF;
				RELAY2_OFF;

				if (resp == RESP_CHANGE_ALL_UP)
				{
					FuncBrdDiagReset();
					main_state = MAIN_INIT;
				}
				break;

			case MAIN_OVERTEMP:
				LCD_1ER_RENGLON;
				LCDTransmitStr((const char *)"    OVERTEMP    ");
				LCD_2DO_RENGLON;
				LCDTransmitStr((const char *)s_blank_line);
				main_state++;
				break;

			case MAIN_OVERTEMP_1:
				if (GetLedTemp() < TEMP_IN_65)
				{
					main_state = last_main_state;
					update_s = UPDATE_YES;
				}
				lcd_backlight_timer = TT_LCD_BACKLIGHT;
				break;

			default:
				main_state = MAIN_INIT;
				break;

		}

		//Reviso la temperatura y muevo el FAN en consecuencia
		if (main_state != MAIN_OVERTEMP_1)
		{
			current_led_temp = GetLedTemp();
			if (current_led_temp > TEMP_IN_65)
			{
				last_main_state = main_state;
				main_state = MAIN_OVERTEMP;
				SetPWMFan(FAN_SPEED_HIGH);
			}
			else if (current_led_temp > TEMP_IN_50)
				SetPWMFan(FAN_SPEED_MED);
			else if (current_led_temp > TEMP_IN_35)
				SetPWMFan(FAN_SPEED_LOW);
			else
				SetPWMFan(FAN_SPEED_OFF);

		}

		UpdateSwitches();
		UpdatePackets();
		UpdateLCDBackLight();
		UpdateFan();
		UpdateLedTemp();

	}	//termina while(1)

	//---------- Fin Programa de PRODUCCION --------//

	return 0;
}
//--- End of Main ---//

void UpdatePackets (void)
{
	if (Packet_Detected_Flag)
	{
		if (data[0] == 0x00)
			DMX_packet_flag = 1;

		if (data[0] == 0xCC)
			RDM_packet_flag = 1;

		Packet_Detected_Flag = 0;
	}
}

unsigned short Get_Temp (void)
{
	unsigned short total_ma;
	unsigned char j;

	//Kernel mejorado ver 2
	//si el vector es de 0 a 7 (+1) sumo todas las posiciones entre 1 y 8, acomodo el nuevo vector entre 0 y 7
	total_ma = 0;
	vtemp[LARGO_FILTRO] = ReadADC1 (ADC_Channel_8);
    for (j = 0; j < (LARGO_FILTRO); j++)
    {
    	total_ma += vtemp[j + 1];
    	vtemp[j] = vtemp[j + 1];
    }

    return total_ma >> DIVISOR;
}

unsigned char MAFilter (unsigned char new_sample, unsigned char * vsample)
{
	unsigned short total_ma;
	unsigned char j;

	//Kernel mejorado ver 2
	//si el vector es de 0 a 7 (+1) sumo todas las posiciones entre 1 y 8, acomodo el nuevo vector entre 0 y 7
	total_ma = 0;
	*(vsample + LARGO_F) = new_sample;

    for (j = 0; j < (LARGO_F); j++)
    {
    	total_ma += *(vsample + j + 1);
    	*(vsample + j) = *(vsample + j + 1);
    }

    return total_ma >> DIVISOR_F;
}

unsigned short MAFilter16 (unsigned char new_sample, unsigned char * vsample)
{
	unsigned short total_ma;
	unsigned char j;

	//Kernel mejorado ver 2
	//si el vector es de 0 a 7 (+1) sumo todas las posiciones entre 1 y 8, acomodo el nuevo vector entre 0 y 7
	total_ma = 0;
	*(vsample + LARGO_F) = new_sample;

    for (j = 0; j < (LARGO_F); j++)
    {
    	total_ma += *(vsample + j + 1);
    	*(vsample + j) = *(vsample + j + 1);
    }

    return total_ma >> DIVISOR_F;
}





void EXTI4_15_IRQHandler(void)
{
	unsigned short aux;


	if(EXTI->PR & 0x0100)	//Line8
	{

		//si no esta con el USART detecta el flanco	PONER TIMEOUT ACA?????
		if ((dmx_receive_flag == 0) || (dmx_timeout_timer == 0))
		//if (dmx_receive_flag == 0)
		{
			switch (signal_state)
			{
				case IDLE:
					if (!(DMX_INPUT))
					{
						//Activo timer en Falling.
						TIM14->CNT = 0;
						TIM14->CR1 |= 0x0001;
						signal_state++;
					}
					break;

				case LOOK_FOR_BREAK:
					if (DMX_INPUT)
					{
						//Desactivo timer en Rising.
						aux = TIM14->CNT;

						//reviso BREAK
						//if (((tim_counter_65ms) || (aux > 88)) && (tim_counter_65ms <= 20))
						//if ((aux > 87) && (aux < 210))	//Consola STARLET 6
						if ((aux > 87) && (aux < 2000))		//Consola marca CODE tiene break 1.88ms
						{
							LED_ON;
							//Activo timer para ver MARK.
							//TIM2->CNT = 0;
							//TIM2->CR1 |= 0x0001;

							signal_state++;
							//tengo el break, activo el puerto serie
							DMX_channel_received = 0;
							//dmx_receive_flag = 1;

							dmx_timeout_timer = DMX_TIMEOUT;		//activo el timer cuando prendo el puerto serie
							//USARTx_RX_ENA;
						}
						else	//falso disparo
							signal_state = IDLE;
					}
					else	//falso disparo
						signal_state = IDLE;

					TIM14->CR1 &= 0xFFFE;
					break;

				case LOOK_FOR_MARK:
					if ((!(DMX_INPUT)) && (dmx_timeout_timer))	//termino Mark after break
					{
						//ya tenia el serie habilitado
						//if ((aux > 7) && (aux < 12))
						dmx_receive_flag = 1;
					}
					else	//falso disparo
					{
						//termine por timeout
						dmx_receive_flag = 0;
						//USARTx_RX_DISA;
					}
					signal_state = IDLE;
					LED_OFF;
					break;

				default:
					signal_state = IDLE;
					break;
			}
		}

		EXTI->PR |= 0x0100;
	}
}

void TimingDelay_Decrement(void)
{
	if (wait_ms_var)
		wait_ms_var--;

//	if (display_timer)
//		display_timer--;

	if (timer_standby)
		timer_standby--;

	if (switches_timer)
		switches_timer--;

	if (dmx_timeout_timer)
		dmx_timeout_timer--;

	if (timer_fan_freerun < TIMER_FAN_ROOF)
		timer_fan_freerun++;
	else
		timer_fan_freerun = 0;

//	if (prog_timer)
//		prog_timer--;

	if (take_temp_sample)
		take_temp_sample--;

	if (take_led_temp_sample)
		take_led_temp_sample--;

	if (filter_timer)
		filter_timer--;

	if (lcd_backlight_timer)
		lcd_backlight_timer--;

	//-------- Timers para funciones de seleccion ---------//
	if (show_select_timer)
		show_select_timer--;

	if (scroll1_timer)
		scroll1_timer--;

	if (scroll2_timer)
		scroll2_timer--;

	//-------- Timers para funciones y sus menues ---------//
	if (function_timer)
		function_timer--;

	if (function_enable_menu_timer)
		function_enable_menu_timer--;

	if (function_save_memory_timer)
		function_save_memory_timer--;

/*
	//cuenta de a 1 minuto
	if (secs > 59999)	//pasaron 1 min
	{
		minutes++;
		secs = 0;
	}
	else
		secs++;
*/
}





