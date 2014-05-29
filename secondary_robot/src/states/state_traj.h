/*
 * state_traj.h
 *
 *  Created on: 8 mai 2013
 *      Author: quentin
 *   Modify on: janvier 2014
 *   		By: Seb
 */

#ifndef STATE_TRAJ_H_
#define STATE_TRAJ_H_

#include "state_types.h"

//#define MUR_R
#define Int_R
//#define MUR_Y
#define INT_Y

typedef struct {
    int speed;      //in inc/s
    int angle;      //in degree, 0°=straight forward
    unsigned long duration;   //in ms
}trajElem;

int progTraj(trajElem tab[]);


extern sState sTrajRedInit;
extern sState sTrajYellowInit;
extern sState sTrajRedFinal;
extern sState sTrajYellowFinal;


int periodicProgTraj(trajElem tab[],unsigned long *pausetime, int *i, unsigned long *prev_millis);

extern sState sTrajRedInit;
extern sState sTrajYellowInit;


#endif /* STATE_TRAJ_H_ */
