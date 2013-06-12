/*
 * lib_radar.h
 *
 *  Created on: 23 avr. 2013
 *      Author: quentin
 */

#ifndef LIB_RADAR_H_
#define LIB_RADAR_H_

#include "Arduino.h"
#include "Servo.h"
#include "lib_us.h"

//defines
#define DEBUG_RADAR

#define RAD_TIMER_1 70    //time between call for mesure and mesure reading (cf sensor datasheet)
#define RAD_TIMER_2 180   //time between mesure reading and call for next mesure (cf servo sweeping speed)


#define RAD_POS_MIN 57
#define RAD_POS_MAX 117
#define RAD_POS_INC 30  //must be a submiltiple of RAD_POS_MAX-RAD_POS_MIN && must be different from zero
#define RAD_NB_POS  ( ((RAD_POS_MAX-RAD_POS_MIN)/RAD_POS_INC)+1 )
#define RAD_NB_PTS  ( 4*RAD_NB_POS)

/* how are stored the values in C_rad and C_rad_limits (0° pointing toward the front of the robot, angles increasing clockwise):
index  angle  direction
0      202,5  rear-left
1      157,5  rear-right
2      112,5  right-front
3      67,5   right-rear
4      22,5   front-right
5      337,5  front-left
6      292,5  left-front
7      247,5  left-rear*/
void radarSetLim(uint16_t[8]);

void radarInitHard(int);

//function to call periodically to refresh the values red by the radar
void radarRefresh();

//returns the shortest range measured
uint16_t radarCloser();

//returns the number of range measures shorter than the limit defined in C_rad_limits
//(for the direction assiciated to each measure).
int radarIntrusion();

#endif /* LIB_RADAR_H_ */
