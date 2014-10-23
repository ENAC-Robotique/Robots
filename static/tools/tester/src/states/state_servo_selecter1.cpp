/*
 * state-blink.cpp
 *
 *  Created on: 15 mai 2013
 *      Author: quentin
 */

#include "Arduino.h"
#include "../params.h"
#include "../tools.h"
#include "state_types.h"

#include "state_Menu_principal.h"
#include "state_Menu_servo.h"
#include "state_servo_selecter1.h"
#include "state_blink.h"



sState* testservo_selecter1(){
	static int memPosition=0;
	int Position = (myEnc.read()/2*5)%185;

	if(!digitalRead(SELECT))	//nécessite de valider avant que le servo ne se déplace
	{
		servotest.write(Position);
	}

	if(Position!=memPosition)
	{
		char affich[16];
		snprintf(affich,17,"Angle= %d",Position);
		afficher(affich);
		memPosition=Position;
	}

	/*if(retour)
	{
		retour=0;
		memenc=myEnc.read();
		return(&sMenu_servo);
	}*/
	if(!digitalRead(RETOUR))
	{
		delay(3);	//anti rebond
		while(!digitalRead(RETOUR));	//attente du relachement du bouton
		return(&sMenu_servo);
	}
    return NULL;
}
void initservo_selecter1(sState *prev){
	servotest.attach(PIN_PWM_SERVO);
	int angle=servotest.read();
	int value_enc=5*angle/2;
	myEnc.write(value_enc);

}
void deinitservo_selecter1(sState *next){
	servotest.detach();
}

void servo_selecter1(){

}

sState sservo_selecter1={
    0,
    &initservo_selecter1,
    &deinitservo_selecter1,
    &testservo_selecter1
};


