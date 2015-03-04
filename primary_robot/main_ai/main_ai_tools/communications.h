/*
 * communications_tools.h
 *
 *  Created on: 22 févr. 2015
 *      Author: seb
 */

#ifndef COMMUNICATIONS_TOOLS_H_
#define COMMUNICATIONS_TOOLS_H_

#include <fstream>

#include "math_types.h"

using namespace std;

//Send message
void sendPing();
void sendObsCfg();
void sendObss();
int sendPos(sPt_t &p, sNum_t theta);
int sendSpeed(sNum_t speed);

//Receive message
void checkInbox(int verbose, ofstream &file);
bool lastGoal(sPt_t &goal, bool get);

#endif /* COMMUNICATIONS_TOOLS_H_ */