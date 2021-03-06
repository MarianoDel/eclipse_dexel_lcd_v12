/*
 * hard.h
 *
 *  Created on: 28/11/2013
 *      Author: Mariano
 */

#ifndef HARD_H_
#define HARD_H_


//-------- Defines For Configuration -------------
#define VER_1_2


//-------- End Of Defines For Configuration ------


//-------- Estructuras Generales para Configuracion -----------

typedef struct Configuration
{
	//--- Opciones General de modo de funcionamiento ---//
	unsigned char general_mode;						//subtotal 1 bytes

	//--- Opciones de la Funcion Manual ---//
	unsigned char manual_channels_quantity;
	unsigned char manual_ch1_value;
	unsigned char manual_ch2_value;
	unsigned char manual_relays_usage;				//subtotal 4 bytes

	//--- Opciones de la Funcion DMX ---//
	unsigned short dmx_addr;
	unsigned char dmx_channel_quantity;
	unsigned char dmx_relays_usage;					//subtotal 4 bytes

	//--- Opciones de la Funcion COLORS ---//
	unsigned char colors_selected;					//subtotal 1 bytes

	//--- Opciones de la Funcion BRD DIAG ---//
	unsigned short brd_diag_saved_times;			//subtotal 2 bytes


} Configuration_Typedef;	//12 bytes aligned

//MODOS GENERALES DE FUNCIONAMIENTO
#define GENERAL_DMX_MODE		1
#define GENERAL_MANUAL_MODE		2
#define GENERAL_COLORS_MODE		3

//-------- FIN Estructuras Generales para Configuracion -----------




//-------- Defines For PINs Configuration -------------

#ifdef VER_1_2
//GPIOA pin0
//GPIOA pin1
//GPIOA pin2
//GPIOA pin3	interface a LCD

//GPIOA pin4
#define LCD_E ((GPIOA->ODR & 0x0010) != 0)
#define LCD_E_ON	GPIOA->BSRR = 0x00000010
#define LCD_E_OFF GPIOA->BSRR = 0x00100000

//GPIOA pin5
#define LCD_RS ((GPIOA->ODR & 0x0020) != 0)
#define LCD_RS_ON	GPIOA->BSRR = 0x00000020
#define LCD_RS_OFF GPIOA->BSRR = 0x00200000

//GPIOA pin6
//para PWM_CH1

//GPIOA pin7
//para PWM_CH2

//GPIOB pin0
//ADC_Current

//GPIOB pin1
#define S_DOWN ((GPIOB->IDR & 0x0002) == 0)

//GPIOA pin8
#define EXTI_Input ((GPIOA->IDR & 0x0100) != 0)
#define DMX_INPUT EXTI_Input

//GPIOA pin9
//GPIOA pin10	usart tx rx

//GPIOA pin11
#define RELAY1 ((GPIOA->ODR & 0x0800) != 0)
#define RELAY1_ON	GPIOA->BSRR = 0x00000800
#define RELAY1_OFF GPIOA->BSRR = 0x08000000

//GPIOA pin12
#define RELAY2	((GPIOA->ODR & 0x1000) != 0)
#define RELAY2_ON	GPIOA->BSRR = 0x00001000
#define RELAY2_OFF	GPIOA->BSRR = 0x10000000

//GPIOA pin13
//GPIOA pin14

//GPIOA pin15
#define S_UP ((GPIOA->IDR & 0x8000) == 0)	//activo por 0

//GPIOB pin3
#define S_SEL ((GPIOB->IDR & 0x0008) == 0)

//GPIOB pin4

//GPIOB pin5
#define FAN ((GPIOB->ODR & 0x0020) != 0)
#define FAN_ON	GPIOB->BSRR = 0x00000020
#define FAN_OFF GPIOB->BSRR = 0x00200000

//GPIOB pin6
#define CTRL_BKL ((GPIOB->ODR & 0x0040) != 0)
#define CTRL_BKL_ON	GPIOB->BSRR = 0x00000040
#define CTRL_BKL_OFF GPIOB->BSRR = 0x00400000

//GPIOB pin7
#define LED ((GPIOB->ODR & 0x0080) != 0)
#define LED_ON	GPIOB->BSRR = 0x00000080
#define LED_OFF GPIOB->BSRR = 0x00800000


#endif	//

//-------- END of Defines For PINs Configuration -------------

//ESTADOS DEL PROGRAMA PRINCIPAL
//estados del MAIN MENU
enum var_main_states
{
	MAIN_INIT = 0,
	MAIN_DMX,
	MAIN_MANUAL,
	MAIN_COLORS,
	MAIN_BRD_DIAG,
	MAIN_OVERTEMP,
	MAIN_OVERTEMP_1

};


//---- Temperaturas en el LM335
//37	2,572
//40	2,600
//45	2,650
//50	2,681
//55	2,725
//60	2,765
#define TEMP_IN_30		3125
//#define TEMP_IN_35		3169	//2.555 V
#define TEMP_IN_35		3135	//2.555 V corrijo para cuando baja muy rapido
#define TEMP_IN_45		3258	//2.62 V
#define TEMP_IN_50		3302	//2.66 V
#define TEMP_IN_55		3346	//2.69
#define TEMP_IN_65		3434	//2.7678 V
#define TEMP_IN_70		3484	//2.8083
#define TEMP_DISCONECT		4000

//ESTADOS DEL DISPLAY EN RGB_FOR_CAT
#define SHOW_CHANNELS	0
#define SHOW_NUMBERS	1

#define SWITCHES_TIMER_RELOAD	10

#define SWITCHES_ROOF	500		//5 segundos
#define SWITCHES_THRESHOLD_FULL	300		//3 segundos
#define SWITCHES_THRESHOLD_HALF	100		//1 segundo
#define SWITCHES_THRESHOLD_MIN	5		//50 ms

//----- Definiciones para timers ------
#define TTIMER_FOR_CAT_DISPLAY			2000	//tiempo entre que dice canal y el numero
#define TIMER_STANDBY_TIMEOUT_REDUCED	2000	//reduced 2 segs
#define TIMER_STANDBY_TIMEOUT			6000	//6 segundos
#define DMX_DISPLAY_SHOW_TIMEOUT		30000	//30 segundos
#define TT_MENU_ENABLED					30000	//30 segundos
#define TT_LCD_BACKLIGHT					30000	//30 segundos

#define TT_SAVE_MEMORY					10000	//10 segundo para grabar memoria

#define S_FULL		10
#define S_HALF		3
#define S_MIN		1
#define S_NO		0

//defines para los reles
#define RELAY_START	15
#define RELAY_STOP	10

//define para los update de SCREEN
#define UPDATE_NO	0
#define UPDATE_YES	1

//----- Definiciones para el FAN ------
#define TIMER_FAN_ROOF	600
#define FAN_SPEED_OFF	0
#define FAN_SPEED_LOW	200
#define FAN_SPEED_MED	400
#define FAN_SPEED_HIGH	TIMER_FAN_ROOF

//#define TIMER_FAN_ROOF	20
//#define FAN_SPEED_OFF	0
//#define FAN_SPEED_LOW	6
//#define FAN_SPEED_MED	12
//#define FAN_SPEED_HIGH	TIMER_FAN_ROOF






// ------- de los switches -------
void UpdateSwitches (void);
unsigned char CheckSDown (void);
unsigned char CheckSUp (void);
unsigned char CheckSSel (void);

void UpdateLCDBackLight (void);

void UpdateFan (void);
void SetPWMFan (unsigned short);


#endif /* HARD_H_ */
