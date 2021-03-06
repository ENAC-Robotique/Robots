
#include "Arduino.h"
#include "state_types.h"
#include "lib_move.h"
#include "lib_radar.h"
#include "lib_line.h"
#include "lib_wall.h"
#include "lib_attitude.h"
#include "lib_heading.h"
#include "tools.h"
#include "state_pause.h"
#include "lib_trajectory.h"

#define FACTOR_HEADING_ASSERV 1.5

int _backFromPause = 0;

int periodicProgTrajHeading(trajElem tab[],unsigned long *pausetime, int *i, unsigned long *prev_millis){
	static int teta0 = headingGetCon();
	int dt = millis() - *prev_millis -*pausetime;    //time since start of the current traj element
	int dteta = (tab[*i].teta-teta0)*min((dt*FACTOR_HEADING_ASSERV/tab[*i].duration),1);
	headingSetCon( teta0+dteta);

    if (!(*prev_millis)){
    	*prev_millis=millis();
    	speedSetCon(tab[*i].speed);
    }

    if(_backFromPause){
    	_backFromPause = 0;
    	speedSetCon(tab[*i].speed);

    }

    if ( (millis()-*prev_millis-*pausetime)>tab[*i].duration ) {
        (*i)++;
        *prev_millis=millis();
        *pausetime=0;
        teta0 = headingGetCon();
        speedSetCon(tab[*i].speed);
    }
    if ( tab[*i].teta==0 && tab[*i].duration==0 && tab[*i].speed==0) {
        *i=0;
        *prev_millis=0;
        return 1;
    }

    return 0;
}


int periodicProgTraj(const trajElem tab[],unsigned long *pausetime, int *i, unsigned long *prev_millis){
    trajElem elt;/* = tab[*i];*/
    memcpy_P(&elt,&(tab[*i]),sizeof(trajElem));
	if (!(*prev_millis)){
    	*prev_millis=millis();
        move(elt.speed,elt.teta);
    }

    if(_backFromPause){
    	_backFromPause = 0;
    	move(elt.speed,elt.teta);

    }

    if ( (millis()-*prev_millis-*pausetime)>elt.duration ) {
        (*i)++;
        memcpy_P(&elt,&(tab[*i]),sizeof(trajElem));
        *prev_millis=millis();
        *pausetime=0;
        move(elt.speed,elt.teta);
    }
    if ( elt.teta==0 && elt.duration==0 && elt.speed==0) {
        *i=0;
        *prev_millis=0;
        return 1;
    }

    return 0;
}
