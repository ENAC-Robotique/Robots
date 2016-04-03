/*
 * state_Peche.cpp
 *
 *  Created on: 2016 février 05
 *      Author: Darian
 */


#include "Arduino.h"
#include "state_types.h"
#include "lib_move.h"
#include "lib_radar.h"
#include "lib_trajectory.h"
#include "../tools.h"
#include "../params.h"
#include "state_wait.h"
#include "lib_radar_mask.h"
#include "state_funny_action.h"
#include "state_tirette.h"

sState* testPeche(){


#ifdef TIME_FOR_FUNNY_ACTION
	if((millis()-_matchStart) > TIME_FOR_FUNNY_ACTION ) return &sFunnyAction;
#endif

	trajElem purple_fishing[] = {
			{0,0,400},
			{60,0,6450},//canne down
			{0,0,1000},
			{-200,-12,500},//canne up
			{-200,-20,1500},
			{-200,35,2000},
			{-200,-15,2000},
			{0,0,500},//canne down
			{0,0,1000},//crema in
			{0,0,200},//canne up
			{0,0,100},//crema out
			{200,-15,2100},
			{200,35,1500},
			{200,-15,1000},
			{200,0,1000},//canne down
			{60,0,6000},
			{0,0,50000},
	};
	static unsigned long st_saveTime=0;
	static int i=0;
	static unsigned long prev_millis=0;
	static int pos_servo = CANNE_VERTICAL;
	switch (i) {
		case 1:
			canne_servo.write(CANNE_DOWN);
			pos_servo = CANNE_DOWN;
			break;
		case 2:
			if ((millis()-prev_millis)>500 and pos_servo>CANNE_UP){
				pos_servo = max(pos_servo - 5, CANNE_UP);
				canne_servo.write(pos_servo);
			}
			break;
		case 7:
			canne_servo.write(CANNE_DOWN);
			break;
		case 8:
			crema_servo.write(CREMA_IN);
			break;
		case 9:
			canne_servo.write(CANNE_UP);
			break;
		case 10:
			crema_servo.write(CREMA_OUT);
			break;
		case 14:
			canne_servo.write(CANNE_DOWN);
			break;

	}
	if(periodicProgTraj(purple_fishing,&st_saveTime,&i,&prev_millis))
	{
		return &sWait;
	}

	return NULL;
}

void initPeche(sState *prev){
	move(0,0);

	canne_servo.write(CANNE_UP);

	crema_servo.write(CREMA_OUT);
    }

void deinitPeche(sState *next){
        // Your code here !
    }

sState sPeche={
		BIT(E_MOTOR)/*|BIT(E_RADAR)*/,
        &initPeche,
        &deinitPeche,
        &testPeche
};

