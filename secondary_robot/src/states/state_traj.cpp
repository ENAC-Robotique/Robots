
#include "Arduino.h"
#include "state_types.h"
#include "lib_move.h"
#include "lib_radar.h"
#include "lib_trajectory.h"
#include "../tools.h"
#include "../params.h"
#include "state_traj.h"
#include "state_pause.h"
#include "state_Recalage.h"
#include "state_wait.h"

unsigned long st_saveTime=0,st_prevSaveTime=0,st_saveTime_radar=0,st_prevSaveTime_radar=0;
#ifdef HEADING
periodicTraj periodicFunction = &periodicProgTrajHeading;
#else
periodicTraj periodicFunction = &periodicProgTraj;
#endif

void initTrajGreenInit(sState *prev)
	{
		#ifdef DEBUG
			Serial.println(F("debut traj rouge (premier trajet)"));
		#endif

	    if (prev==&sPause)
	    	{
			#ifdef DEBUG
				Serial.println(F("\tback from pause"));
			#endif
	        st_saveTime=millis()-st_saveTime+st_prevSaveTime;
	        st_saveTime_radar=millis()-st_saveTime_radar+st_prevSaveTime_radar;
	        _backFromPause = 1;
	    	}
	    uint16_t limits[RAD_NB_PTS]={30, 30, 30, 30};
	   	    		radarSetLim(limits);
	}

void deinitTrajGreenInit(sState *next)
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

trajElem start_purple[]={
//Début trajectoire vers cabines de plage
		{0,-20,100},
		{-300,20,1100},
		{-400,0,1500},
		{-300,-20,900},
		{-200,0,900},
		{0,0,100},//1ere porte fermée
		{300,0,1300},
		{0,90,400},
		{300,90,1200},
		{0,0,400},
		{300,0,1800},
		{0,-90,400},
		{300,-90,1300},
		{0,0,400},
		{-300,0,1000},
		{-200,0,1600},
		{0,0,100},//2eme porte fermée
		{300,0,1400},
		{0,90,400},
		{300,90,1300},
		{0,-35,300},
		{-300,-35,2325},
		{0,0,0},
};
trajElem start_green[]={
		//Début trajectoire vers cabines de plage
						{0,-15,100},
						{-300,-15,1100},
						{-400,0,1500},
						{-300,25,900},
						{-200,0,1050},
						{0,0,100},//1ere porte fermée
						{300,0,1300},
						{0,-90,400},
						{300,-90,1400},
						{0,0,400},//Radar active 9
						{300,0,1700},
						{0,90,400},//Radar inactive 11
						{300,90,1300},
						{0,0,400},
						{-300,0,1000},
						{-200,0,1500},
						{0,0,100},//2eme porte fermée
						{300,0,1400},
						{0,-90,400},
						{300,-90,1300},
						{0,35,300},
						{-300,35,2350},
						{0,0,0},
				};


sState *testTrajGreenInit()
	{
	static int i=0;
    static unsigned long prev_millis=0;
    static int flag_end = 0;
    	if(!flag_end){
		    if(periodicFunction(start_green,&st_saveTime,&i,&prev_millis)){
				#ifdef DEBUG
					Serial.println(F("\tTrajet 1 fini !"));
				#endif
		    	flag_end = 1;
		    }
    	}
    	else{
    		move(-500,0);
		    if (digitalRead(PIN_SWITCH_LEFT) && digitalRead(PIN_SWITCH_RIGHT)){
				move(0,0);
				return &sRecalage;
			}
    	}
    	switch(i){
    		case 9:
    			sTrajGreenInit.flag |= BIT(E_RADAR);
    			break;
    		case 11:
				sTrajGreenInit.flag &= ~BIT(E_RADAR);
				break;
    	}
		 if (radarIntrusion()) return &sPause;
	    return 0;
	}

sState sTrajGreenInit={
        BIT(E_MOTOR),
        &initTrajGreenInit,
        &deinitTrajGreenInit,
        &testTrajGreenInit
};
//*****************************************************************************************************************


void initTrajPurple(sState *prev)
	{
		    #ifdef DEBUG
				Serial.println(F("debut traj violet"));
			#endif


		    if (prev==&sPause)
		    	{
				#ifdef DEBUG
					Serial.println(F("\tback from pause"));
				#endif
		        st_saveTime=millis()-st_saveTime+st_prevSaveTime;
		        _backFromPause = 1;
		    	}
		    uint16_t limits[RAD_NB_PTS]={40, 40, 40, 40};
		        radarSetLim(limits);
	}

void deinitTrajPurpleInit(sState *next)
	{
		    if (next==&sPause)
		    	{
		        st_prevSaveTime=st_saveTime;
		        st_saveTime=millis();
		    	}
		    else
		    	{
#ifdef DEBUG
	Serial.println(F("Fin traj jaune"));
#endif
		        st_saveTime=0;
		        st_prevSaveTime=0;
		    	}
}



sState *testTrajPurple()
	{
	static int i=0;
    static unsigned long prev_millis=0;
    static int flag_end = 0;
    	if(!flag_end){
		    if(periodicFunction(start_purple,&st_saveTime,&i,&prev_millis)){
				#ifdef DEBUG
					Serial.println(F("\tTrajet 1 fini !"));
				#endif
		    	flag_end = 1;
		    }
    	}
    	else{
    		move(-500,0);
		    if (digitalRead(PIN_SWITCH_LEFT) && digitalRead(PIN_SWITCH_RIGHT)){
				move(0,0);
				return &sRecalage;
			}
    	}

		 if (radarIntrusion()) return &sPause;
	    return 0;
	}
sState sTrajPurpleInit={
		BIT(E_MOTOR)/*|BIT(E_RADAR)*/,
        &initTrajPurple,
        &deinitTrajPurpleInit,
        &testTrajPurple
};


