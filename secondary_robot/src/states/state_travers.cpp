
#include "Arduino.h"
#include "state_types.h"
#include "lib_move.h"
#include "lib_radar.h"
#include "lib_trajectory.h"
#include "../tools.h"
#include "../params.h"
#include "state_travers.h"
#include "state_pause.h"
#include "state_funny_action.h"
#include "state_wait.h"
#include "state_dead.h"

unsigned long st_saveTime=0,st_prevSaveTime=0,st_saveTime_radar=0,st_prevSaveTime_radar=0;
#ifdef HEADING
periodicTraj periodicFunction = &periodicProgTrajHeading;
#else
static periodicTraj periodicFunction = &periodicProgTraj;
#endif

static unsigned long pause_time =0;
static unsigned long start_pause=0;
Servo Hodor;
#define TIME_TO_TRAVEL 75000

void TraversYellowInit(sState *prev)
{
#ifdef DEBUG
	Serial.println(F("debut traverser yellow (premier trajet)"));
#endif

	if (prev==&sPause)
	{
#ifdef DEBUG
		Serial.println(F("\tback from pause"));
#endif
		st_saveTime=millis()-st_saveTime+st_prevSaveTime;
		st_saveTime_radar=millis()-st_saveTime_radar+st_prevSaveTime_radar;
		_backFromPause = 1;
		pause_time+=(millis()-start_pause);
	}
	uint16_t limits[RAD_NB_PTS]={25,0,0,0};
	radarSetLim(limits);
#ifdef DEBUG
	Serial.println(pause_time);
#endif

}

void TraversYellowDeinit(sState *next)
{
	if (next==&sPause)
	{
		st_prevSaveTime=st_saveTime;
		st_saveTime=millis();
		st_prevSaveTime_radar=st_saveTime_radar;
		st_saveTime_radar=millis();
	}
	else
	{
		st_saveTime=0;
		st_prevSaveTime=0;
		st_saveTime_radar=0;
		st_prevSaveTime_radar=0;
		move(0,0);
	}
}

const PROGMEM trajElem trav_blue[]={
		//Début trajectoire blue
		DEMI_TOUR_NEG,//Turn
		(0,0,0),
};


const PROGMEM trajElem trav_yellow[]={
		//Début trajectoire yellow
		{200,0,150,DISTANCE},
		(0,0,0),
};


sState *TraversYellowTest()
{
	static int i=0; //indice de la pos ds la traj
	static unsigned long prev_millis=0;

	uint16_t limits[RAD_NB_PTS]={0,0,0, 0};

#ifdef TIME_FOR_FUNNY_ACTION
	if((millis()-_matchStart) > TIME_FOR_FUNNY_ACTION ) return &sFunnyAction;
#endif
	if (periodicFunction(trav_yellow,&st_saveTime,&i,&prev_millis))
	{
#ifdef DEBUG
		Serial.println(F("\tTravers jaune fini !"));
#endif
		move(0,0);
		return &sDead;
	}
	/*
	if (radarIntrusion())
	 {
		 start_pause=millis();
		 return &sDead;
	 }*/
	return 0;
}

sState sTraverseYellow={
		BIT(E_MOTOR)/*|BIT(E_RADAR)*/,
		&TraversYellowInit,
		&TraversYellowDeinit,
		&TraversYellowTest
};
//*****************************************************************************************************************


void TraversBlueInit(sState *prev)
{
#ifdef DEBUG
	Serial.println(F("debut traverser blue"));
#endif

	if (prev==&sPause)
	{
#ifdef DEBUG
		Serial.println(F("\tback from pause"));
#endif
		st_saveTime=millis()-st_saveTime+st_prevSaveTime;
		_backFromPause = 1;
		pause_time+=(millis()-start_pause);
	}
	uint16_t limits[RAD_NB_PTS]={0, 20, 0, 0};
	radarSetLim(limits);
}

void TraversBlueDeinit(sState *next)
{
	if (next==&sPause)
	{
		st_prevSaveTime=st_saveTime;
		st_saveTime=millis();
		st_prevSaveTime_radar=st_saveTime_radar;
		st_saveTime_radar=millis();
	}
	else
	{
		st_saveTime=0;
		st_prevSaveTime=0;
		st_saveTime_radar=0;
		st_prevSaveTime_radar=0;
	}
}

sState *TraversBlueTest()
{
	static int i=0;
	static unsigned long prev_millis=0;

	static int flag_end = 0;
	//uint16_t limits[RAD_NB_PTS]={0,0,0, 0};

#ifdef TIME_FOR_FUNNY_ACTION
	if((millis()-_matchStart) > TIME_FOR_FUNNY_ACTION ) return &sFunnyAction;
#endif

	if(!flag_end){
		if(periodicFunction(trav_blue,&st_saveTime,&i,&prev_millis)){
#ifdef DEBUG
			Serial.println(F("\tTravers blue fini !"));
#endif


			Hodor.write(HODOR_OPEN);
			flag_end = 1;
			pause_time=0;
			move(0,0);
			return &sDead;
		}
	}
	return 0;
}

sState sTraverseBlue={
		BIT(E_MOTOR)/*|BIT(E_RADAR)*/,
		&TraversBlueInit,
		&TraversBlueDeinit,
		&TraversBlueTest
};

