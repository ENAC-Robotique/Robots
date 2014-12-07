

#include "Arduino.h"
#include "../tools.h"
#include "../params.h"
#include "state_pause.h"
#include "state_funny.h"
#include "lib_radar2.h"
#include "lib_move.h"
#include "state_tirette.h"

sState *pausePrevState;
sState* testPause(){
    static unsigned long lastSeen;
    if(radarIntrusion()) lastSeen=millis();
    if( (millis()-lastSeen)>= RADAR_SAFETY_TIME ) return pausePrevState;
    if ((millis()-_matchStart) > TIME_MATCH_STOP ) return &sFunny;
    if ((millis()-_matchStart) > TIME_MATCH_LAUN ) {
	launcherServoUp.write(10);
	launcherServoNet.write(LAUNCHER_NET_POS_1);}
    return 0;
}


void initPause(sState *prev){
    pausePrevState=prev;
    move(0,0);
    Serial.println("je suis en pause");
#ifdef DEBUG
    Serial.println("debut pause");
#endif
}

void deinitPause(sState *next){
#ifdef DEBUG
    Serial.println("fin pause");
#endif
}

sState sPause={
    BIT(E_MOTOR) | BIT(E_RADAR),
    &initPause,
    &deinitPause,
    &testPause
};
