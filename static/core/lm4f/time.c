/*
 ************************************************************************
 *	time.c
 *
 *	arduino-like time functions for stellaris launchpad
 *
 *
 ***********************************************************************

  Derived from :
  wiring.c - Arduino core files for MSP430
  Copyright (c) 2012 Robert Wessels. All right reserved.

  Derived from:
  wiring.c - Partial implementation of the Wiring API for the ATmega8.
  Part of Arduino - http://www.arduino.cc/

  Copyright (c) 2005-2006 David A. Mellis

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA  02111-1307  USA
 */
#include <stdint.h>
#include "time.h"
#include "inc/hw_memmap.h"
#include "inc/hw_nvic.h"
#include "inc/hw_types.h"

#include "inc/hw_ints.h"
#include "inc/hw_timer.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/systick.h"

static void (*SysTickCbFuncs[8])(uint32_t ui32TimeMS);

#define SYSTICKMS               (1000 / SYSTICKHZ)
#define SYSTICKHZ               100

static unsigned long milliseconds = 0;
#define SYSTICK_INT_PRIORITY    0x80


void SysTickIntHandler(void)
{
	uint8_t i;
	for (i=0; i<8; i++) {
		if (SysTickCbFuncs[i])
			SysTickCbFuncs[i](SYSTICKMS);
	}
}

void Timer5IntHandler(void)
{

    ROM_TimerIntClear(TIMER5_BASE, TIMER_TIMA_TIMEOUT);
	milliseconds++;
}

void timerInit()
{
#ifdef TARGET_IS_BLIZZARD_RA1
    //
    //  Run at system clock at 80MHz
    //
    SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|
                SYSCTL_OSC_MAIN);
#else
    //
    //  Run at system clock at 120MHz
    //
    SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ|SYSCTL_OSC_MAIN|SYSCTL_USE_PLL|SYSCTL_CFG_VCO_480), F_CPU);
#endif

    //
    //  SysTick is used for delay() and delayMicroseconds()
    //

    ROM_SysTickPeriodSet(F_CPU / SYSTICKHZ);
    ROM_SysTickEnable();
    ROM_IntPrioritySet(FAULT_SYSTICK, SYSTICK_INT_PRIORITY);
    SysTickIntRegister(SysTickIntHandler); // missing in energia's original
    ROM_SysTickIntEnable();

    //
    //Initialize Timer5 to be used as time-tracker since beginning of time
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER5); //not tied to launchpad pin
    ROM_TimerConfigure(TIMER5_BASE, TIMER_CFG_PERIODIC_UP);

    ROM_TimerLoadSet(TIMER5_BASE, TIMER_A, F_CPU/1000);

    TimerIntRegister(TIMER5_BASE,TIMER_A,Timer5IntHandler); // also missing in original

    ROM_IntEnable(INT_TIMER5A);
    ROM_TimerIntEnable(TIMER5_BASE, TIMER_TIMA_TIMEOUT);

    ROM_TimerEnable(TIMER5_BASE, TIMER_A);

    ROM_IntMasterEnable();

}

unsigned long micros(void)
{
	return (milliseconds * 1000) + (HWREG(TIMER5_BASE + TIMER_O_TAV) / (F_CPU/1000000));
}

unsigned long millis(void)
{
	return milliseconds;
}

void delayMicroseconds(unsigned int us)
{
	volatile unsigned long elapsedTime;
	unsigned long startTime = HWREG(NVIC_ST_CURRENT);
	do{
		elapsedTime = startTime-(HWREG(NVIC_ST_CURRENT) & 0x00FFFFFF);
	}
	while(elapsedTime <= us * (F_CPU/1000000));
}

void delay(uint32_t milliseconds)
{
		unsigned long i;
		for(i=0; i<milliseconds; i++){
			delayMicroseconds(1000);
		}
}


void registerSysTickCb(void (*userFunc)(uint32_t))
{
	uint8_t i;
	for (i=0; i<8; i++) {
		if(!SysTickCbFuncs[i]) {
			SysTickCbFuncs[i] = userFunc;
			break;
		}
	}
}
