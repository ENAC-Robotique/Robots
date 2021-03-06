/*
 * types.h
 *
 *  Created on: 23 avr. 2013
 *      Author: quentin
 */

#ifndef TYPES_H_
#define TYPES_H_


typedef enum {
    E_BLINK,
    E_RADAR,
    E_MOTOR,
    E_WALL, //(c) sneydi
    E_LINE,
    E_ATTITUDE,
    E_HEADING,
}eBlocks;

typedef enum {
	X_ANGLE,
	Y_ANGLE,
	Z_ANGLE,
	X_ACCEL,
	Y_ACCEL,
	Z_ACCEL,
	X_ANGLE_ACCEL,
	Y_ANGLE_ACCEL,
	Z_ANGLE_ACCEL
}eInertial;

//pointeur de fonction
typedef int(*pfi)();
typedef void(*pfvps)(struct sState*);

typedef struct sState*(*pfps)();


//état élémentaire :
struct sState{
    unsigned int flag;
    pfvps init;
    pfvps deinit;
    pfps testFunction;
};

typedef struct sState sState;


#endif /* TYPES_H_ */
