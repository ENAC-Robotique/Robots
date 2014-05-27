/*
 * absolutepos.c
 *
 *  Created on: 25 mai 2014
 *      Author: ThomasDq
 */

#include <absolutepos.h>
#include <lib_synchro_beacon.h>
#include <messages.h>
#include <network_cfg.h>
#include <params.h>

#include "../../../communication/botNet/shared/message_header.h"

typedef enum{INIT,COLORDETEC,PLAY} EBaliseState;
EBaliseState state = INIT;


/*
 * TODO calcperception
 * TODO sendmessage (sendrole)

 */


/* (laser tourne en sens horaire)
 *
 * cas rouge
 * 2--/--------------\--|
 * |R/                \J|
 * |/                  \3    => 1 2 3
 * |                    |
 * 1--------------------|
 *
 * cas jaune
 * |--/--------------\--2
 * |R/                \J|
 * 3/                  \|    => 3 2 1
 * |                    |
 * |--------------------1
 *
 * ^
 * |--/--------------\--|
 * Y R                J |
 * |/                  \|
 * |                    |
 * |--------------------X->
 *
 *
 */
typedef struct{
	sPt_t pt;
	unsigned long date;
}sDatedPt_t;

// return apporximated position from the two last positions
void approxPos(unsigned long current_date, sDatedPt_t *Z1, sDatedPt_t *Z2, sPt_t *res){
	res->x = Z2->pt.x + (current_date - Z2->date) / (Z2->date - Z1->date) * (Z2->pt.x - Z1->pt.x);
	res->y = Z2->pt.y + (current_date - Z2->date) / (Z2->date - Z1->date) * (Z2->pt.y - Z1->pt.y);
}

void absolutepos(sMeasures*buffer,int index, int taille) {

	sDatedPt_t LastPos[2];
	sPt_t x0;
	int i;
	int idx[3];

#define SWAG_SWAP(i, j)\
	do{\
		int tmp;\
		tmp = idx[i];\
		idx[i] = idx[j];\
		idx[j] = tmp;\
	}while(0)

	// Sorting of index XXX

	idx[0] = (taille + index - 3)%taille;
	idx[1] = (taille + index - 2)%taille;
	idx[2] = (taille + index - 1)%taille;


	if(buffer[idx[0]].date > buffer[idx[1]].date){
		SWAG_SWAP(0,1);
	}
	if(buffer[idx[1]].date > buffer[idx[2]].date){
		SWAG_SWAP(1,2);
		if(buffer[idx[0]].date > buffer[idx[1]].date){
			SWAG_SWAP(0,1);

		}
	}

	bn_printfDbg("%lu,%lu,%lu;%lu,%lu,%lu;%lu,%lu,%lu\n", buffer[idx[0]].deltaT, buffer[idx[1]].deltaT, buffer[idx[2]].deltaT, buffer[idx[0]].period/1000, buffer[idx[1]].period/1000, buffer[idx[2]].period/1000, buffer[idx[0]].date, buffer[idx[1]].date, buffer[idx[2]].date);

	// Initialisation

	switch(state){
	case INIT:
		// wait for 3 available info
		if (buffer[idx[0]].beacon != 0) {
			state = COLORDETEC;
		}

		break;
	case COLORDETEC:
		// Determine team's color

		// current head back_up

		//test that the 3 last informations are from different beacons
		if (BIT(buffer[idx[0]].beacon)
				+ BIT(buffer[idx[1]].beacon)
				+ BIT(buffer[idx[2]].beacon)
				== 7){

			//test if all beacon were shot in the same lasor rotation period
			if( (buffer[idx[2]].date - buffer[idx[0]].date)
					< (buffer[idx[2]].period + (buffer[idx[2]].period>>1)) ){
				int test_result = 1;
				/*
				 *if team red possible beacon number orders
				 *  1 2 3 ; 2 3 1; 3 1 2
				 *
				 * if team yellow
				 *	3 2 1 ; 2 1 3; 1 3 2
				 *
				 * test verified by red possible orders and not by yellow ones
				 *  n.(i) [3] < n.(i+1)
				 */
				for( i = 0; i <3; i++){
					test_result = test_result & ( (buffer[idx[i]].beacon % 3) < buffer[idx[(i+1)%3]].beacon);
				}
				// cas RED
				if (test_result){
					init_globals(RED, &x0);
					bn_printDbg("red case\n");
				}
				// cas YELLOW
				else{
					init_globals(YELLOW, &x0);
					bn_printDbg("yellow case\n");
				}
				state = PLAY;
				for(i=0 ; i<2;i++){
					LastPos[i].pt = x0; // XXX C'est pas bô
				}
			}

			break;
	case PLAY:

		//test that the 3 last informations are from different beacons
		if (BIT(buffer[idx[0]].beacon)
				+ BIT(buffer[idx[1]].beacon)
				+ BIT(buffer[idx[2]].beacon)
				== 7){

			//test if all beacon were shot in the same lasor rotation period ( < 1.5 period)
			if( (buffer[idx[2]].date - buffer[idx[0]].date)
					< (buffer[idx[0]].period + (buffer[idx[0]].period>>1))
			){
				sPerception perception;
				calcPerception(&perception,&(buffer[idx[0]]),&(buffer[idx[1]]),&buffer[idx[2]]);
				approxPos( buffer[idx[2]].date, &LastPos[0],&LastPos[1], &x0);
				if (neldermead(&x0, RANGE, &perception)){
					LastPos[0] = LastPos[1];
					LastPos[1].pt = x0;
					LastPos[1].date = buffer[idx[2]].date;

					//renvoi position
					sMsg msg_out;

					// Header
					msg_out.header.destAddr = role_get_addr(ROLE_IA);
					msg_out.header.type = E_GENERIC_STATUS;
					msg_out.header.size = sizeof(sGenericStatus);

					// Payload
					msg_out.payload.genericStatus.date = micros2s(buffer[idx[2]].date);
					msg_out.payload.genericStatus.id = ELT_PRIMARY;
					msg_out.payload.genericStatus.pos.x = x0.x;
					msg_out.payload.genericStatus.pos.y = x0.y;
					msg_out.payload.genericStatus.pos.frame = FRAME_PLAYGROUND;
					msg_out.payload.genericStatus.pos_u.a_std = 0;
					msg_out.payload.genericStatus.pos_u.b_std = 0;
					msg_out.payload.genericStatus.pos_u.a_angle = 0;
					msg_out.payload.genericStatus.pos_u.theta = -1;

					//					bn_send(&msg_out);
#ifdef DEBUG_POS
					bn_printfDbg("date %lu, x: %d, y: %d\n",msg_out.payload.genericStatus.date,(int)x0.x,(int)x0.y);
#endif


				}else{
					LastPos[0] = LastPos[1];
				}
			}
		}

		break;

		}
	}

#undef SWAG_SWAP
}

