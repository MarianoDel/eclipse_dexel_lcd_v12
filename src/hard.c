/*
 * hard.c
 *
 *  Created on: 11/02/2016
 *      Author: Mariano
 */
#include "hard.h"
#include "stm32f0xx.h"

// ------- Externals de los switches -------
extern unsigned short sdown;
extern unsigned short sup;
extern unsigned short ssel;

// ------- Externals de los timers -------
extern volatile unsigned char switches_timer;
extern volatile unsigned short timer_fan_freerun;

// ------- Globales del Modulo -------
unsigned short pwm_fan = 0;

// ------- Funciones del Modulo -------

void UpdateSwitches (void)
{
	//revisa los switches cada 10ms
	if (!switches_timer)
	{
		if (S_DOWN)
			sdown++;
		else if (sdown > 50)
			sdown -= 50;
		else if (sdown > 10)
			sdown -= 5;
		else if (sdown)
			sdown--;

		if (S_UP)
			sup++;
		else if (sup > 50)
			sup -= 50;
		else if (sup > 10)
			sup -= 5;
		else if (sup)
			sup--;

		if (S_SEL)
			ssel++;
		else if (ssel > 50)
			ssel -= 50;
		else if (ssel > 10)
			ssel -= 5;
		else if (ssel)
			ssel--;

		switches_timer = SWITCHES_TIMER_RELOAD;
	}
}

void UpdateFan (void)
{
	//armo un PWM
	if (timer_fan_freerun < pwm_fan)
		FAN_ON;
	else
		FAN_OFF;
}

void SetPWMFan (unsigned short a)
{
	//armo un PWM
	if (a < TIMER_FAN_ROOF)
		pwm_fan = a;
	else
		pwm_fan = TIMER_FAN_ROOF;
}

unsigned char CheckSUp (void)	//cada check tiene 12ms
{
	if (sup > SWITCHES_THRESHOLD_FULL)
		return S_FULL;

	if (sup > SWITCHES_THRESHOLD_HALF)
		return S_HALF;

	if (sup > SWITCHES_THRESHOLD_MIN)
		return S_MIN;

	return S_NO;
}

unsigned char CheckSDown (void)	//cada check tiene 10ms
{
	if (sdown > SWITCHES_THRESHOLD_FULL)
		return S_FULL;

	if (sdown > SWITCHES_THRESHOLD_HALF)
		return S_HALF;

	if (sdown > SWITCHES_THRESHOLD_MIN)
		return S_MIN;

	return S_NO;
}

unsigned char CheckSSel (void)
{
	if (ssel > SWITCHES_THRESHOLD_FULL)
		return S_FULL;

	if (ssel > SWITCHES_THRESHOLD_HALF)
		return S_HALF;

	if (ssel > SWITCHES_THRESHOLD_MIN)
		return S_MIN;

	return S_NO;
}

