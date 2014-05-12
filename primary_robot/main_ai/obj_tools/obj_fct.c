/*
 * obj_fct.c
 *
 *  Created on: 29 mars 2014
 *      Author: seb
 */

#include "obj_fct.h"

void printServoPos(eServoPos_t *pos){
    switch(*pos){
        case CLOSE:
            printf("CLOSE");
            break;
        case HALF_OPEN:
            printf("HALF_OPEN");
            break;
        case OPEN:
            printf("OPEN");
            break;
        default:
            printf("Error in switch printServoPos\n");
        }
    }

void printListObj(void){
    int i,j;
    printf("ListObj :\n");
    for(i=0 ; i<NB_OBJ ; i++){
        printf("type=%d, ",listObj[i].type);
        printf("numObj=%d, ",listObj[i].numObj);
        printf("nbObs=%d, ",listObj[i].nbObs);
        printf("numObs={");
        for(j=0 ; j<listObj[i].nbObs ; j++) printf("%d, ",listObj[i].numObs[j]);
        printf("}, ");
        printf("dist=%f, ",listObj[i].dist);
        printf("active=%d, ",listObj[i].active);
        printf("done=%f, ",listObj[i].done);
        printf("nbEP=%d, ",listObj[i].nbEP);
        printf("entryPoint={");
        for(j=0 ; j<listObj[i].nbEP ; j++){
        	printf("{{%d, %d}, ",(int)listObj[i].entryPoint[j].c.x,(int)listObj[i].entryPoint[j].c.y);
        	printf("%d, ",(int)listObj[i].entryPoint[j].radiusEP);
        	printf("%d}, ",listObj[i].entryPoint[j].angleEP);
        	}
        printf("}, ");
        printf("\n");
        }
    printf("\n");
    }

void printObsActive(void){
	int i;
	printf("Liste des obs[i].active :\n");
	for(i=0 ; i<N ; i++)
		printf("obs[%d].active=%d\n",i,obs[i].active);
	printf("\n");
	}


void init_ele(void){
    printf("Debut de l'initialisation des elements du jeu\n");
    int i, j;

    //Initialisation des arbres
    for(i=0 ; i<4 ; i++){
    	listObj[i].typeStruct = &arbre[listObj[i].numObj];
    	((Obj_arbre*)listObj[i].typeStruct)->nb_point=10;                      //1 fruit pouri par arbre
    	for(j=0 ; j<6 ; j++) ((Obj_arbre*)listObj[i].typeStruct)->eFruit[j]=0; //par défaut tous les fruit sont bon
    	}
    ((Obj_arbre*)listObj[0].typeStruct)->angle = 90.;
    ((Obj_arbre*)listObj[1].typeStruct)->angle = 180.;
    ((Obj_arbre*)listObj[2].typeStruct)->angle = 180.;
    ((Obj_arbre*)listObj[3].typeStruct)->angle = 270.;
    //Add Entry Point in struct arbre
    for(j=0 ; j<4 ; j++){
    	arbre[j].EntryPoint1=listObj[j].entryPoint[0].c;
    	arbre[j].EntryPoint2=listObj[j].entryPoint[1].c;
    	}

    //Initialisation des bacs
    for(i=4 ; i<6 ; i++){
    	listObj[i].typeStruct = &bac;
        ((Obj_bac*)listObj[i].typeStruct)->nb_point=0;
    	}
    if(color==1)listObj[4].active=0;
    else listObj[5].active=0;


    //Initialisation des feux
    for(i=6 ; i<16 ; i++){
    	listObj[i].typeStruct = &feu[listObj[i].numObj];
        ((Obj_feu*)listObj[i].typeStruct)->nb_point=2;
        listObj[i].nbEP=3;
        createEPfire2(i);
        }

	#if DEBUG
		printListObj();
	#endif
    printf("Fin de l'initialisation des elements du jeu\n");
    }


int get_position( sPt_t *pos){
	*pos = obs[0].c;
    obs_updated[0]++;
	return 1;
    }

int test_in_obs(sPt_t *p){ //retourne le numéros de l'obstable si la position est a l'interieur de celui ci
    //FIXME si le robot dans plusieurs obstable
    int i;
    sNum_t dist;
    for(i=1; i<N-1;i++){
        if(obs[i].active==0)continue;
        distPt2Pt(&obs[i].c, p, &dist);
        if(  dist < obs[i].r){
        	//printf("Le robot est dans l'obstacle n=%i, robs=%f, xobs=%f, yobs=%f, currentpos x=%f, y=%f\n",i,obs[i].r,obs[i].c.x,obs[i].c.y, _current_pos.x, _current_pos.y);
            return i;
            }
        }
    return 0;
    }

int test_tirette(void){
#if SIMU
    return 1;
#endif
    return starting_cord;
    }

void startColor(void){

#if SIMU
    color=COLOR_SIMU;
#else
    static int state = 0;
    sMsg msgOut;

    switch(state){
        case 0 :
            if(mode_switch == 0){ //Red color
                state = 1;
                color = 0;

                msgOut.header.destAddr = ADDRI_MAIN_IO;
                msgOut.header.type = E_IHM_STATUS;
                msgOut.header.size = 2 + 1*sizeof(*msgOut.payload.ihmStatus.states);

                msgOut.payload.ihmStatus.nb_states = 1;
                msgOut.payload.ihmStatus.states[0].id = IHM_LED;
                msgOut.payload.ihmStatus.states[0].state = 1;

                bn_send(&msgOut);
                }
            break;
        case 1 :

            if(mode_switch == 1){
                state = 2;
                }
            break;
        case 2 :
            if(mode_switch == 0){ //Yellow color
                state = 3;
                color = 1;

                msgOut.header.destAddr = ADDRI_MAIN_IO;
                msgOut.header.type = E_IHM_STATUS;
                msgOut.header.size = 2 + 1*sizeof(*msgOut.payload.ihmStatus.states);

                msgOut.payload.ihmStatus.nb_states = 1;
                msgOut.payload.ihmStatus.states[0].id = IHM_LED;
                msgOut.payload.ihmStatus.states[0].state = 2;

                bn_send(&msgOut);
                }
            break;
        case 3 :
            if(mode_switch == 1){
                state = 0;
                }
            break;
        }
#endif
    }

sPt_t trjS[4]={ //trajectory of the secondary robot
    {10. , 190.},
    {40. , 160.},
    {135., 160.},
    {135., 190.}
    };

int testPtInPt(sPt_t *p1, sPt_t *p2, int r){
    return sqrt((p2->x - p1->x) * (p2->x - p1->x) + (p2->y - p1->y) * (p2->y - p1->y)) <= r ;
    }

void simuSecondary(void){ //TODO if a other robot on trajectory
    static int state = 0;
    static unsigned int lastTime = 0;
    static sPt_t pos ;

    unsigned int time = millis();
    sNum_t theta;

    if( !lastTime ){
        lastTime = millis();
        pos = trjS[0];
        }

    if( state < (sizeof(trjS)/sizeof(*trjS) - 1)){
        theta = atan2( (trjS[state+1].y - trjS[state].y), (trjS[state+1].x - trjS[state].x) );
        pos.x += (SPEED_SECONDARY * (time - lastTime) * cos(theta))/1000;
        pos.y += (SPEED_SECONDARY * (time - lastTime) * sin(theta))/1000;

        if( testPtInPt(&pos, &trjS[state+1], 1) ){
            state++ ;
            }

        obs[1].c = pos;
        obs_updated[1]++;

        lastTime = millis();
        }
	}

void posPrimary(void){
    int i;
    sPt_t pt;
    sVec_t v;

    if(get_position(&_current_pos)){
        if(((i=test_in_obs(&_current_pos))!=0) ){
            if( obs[i].moved == 1){
                pt = obs[0].c;
                projPtOnCircle(&obs[i].c, obs[i].r, &pt);
                convPts2Vec(&pt, &obs[0].c, &v);

                obs[i].c.x += v.x;
                obs[i].c.y += v.y;

                obs_updated[i]++;
                }
            project_point(_current_pos.x, _current_pos.y, obs[i].r, obs[i].c.x,obs[i].c.y, &_current_pos);
            if(sqrt(pow(_current_pos.x-obs[0].c.x,2)+pow(_current_pos.y-obs[0].c.y,2)<2)){
                memcpy(&obs[0].c,&_current_pos, sizeof(obs[0].c));
                obs_updated[0]++;
                }
            else{
                memcpy(&_current_pos,&obs[0].c, sizeof(obs[0].c));
                obs_updated[0]++;
                }
            }
        updateNoHaftTurn(theta_robot*180/M_PI, &obs[0].c);
        obs_updated[N-5]++;
        obs_updated[N-6]++;
        obs_updated[N-7]++;
        }
    }

void updateRatioObj(int numObj, int robot){ //robot = 1 to 3
    static int mat[3][NB_OBJ] = {{0, 0}};  //3 is the number of other robot

    if( !mat[robot-1][numObj] ){
        listObj[numObj].done += (1 - listObj[numObj].done)/2;
        mat[robot-1][numObj] = 1;
        }
    }

void checkRobot2Obj(void){
    int i, j, k;
    sNum_t dist;
    static int tab[NB_OBJ][2] ={{0, 0}};

    for(i = 0 ; i < NB_OBJ ; i++){
        if(tab[i][0]){
            tab[i][0] = 0;
            listObj[i].active = tab[i][1];
            }
        for(j = 1 ; j < 4 ; j++){
            for(k = 0 ; k < listObj[i].nbObs ; k++){
                distPt2Pt(&obs[listObj[i].numObs[k]].c, &obs[j].c, &dist);
                if( (obs[listObj[i].numObs[k]].r + obs[j].r - R_ROBOT) > (dist - ERR_DIST) ){
                    updateRatioObj(i, j);
                    tab[i][0] = 1;
                    tab[i][1] = listObj[i].active;
                    listObj[i].active = 0;
                    }
                }
            }
        }
    }

int checkAdvOnRobot(void){
    int i;
    sNum_t dist;

    for(i = 1 ; i < 3 ; i++){
        distPt2Pt(&obs[i].c, &obs[0].c, &dist);
        if( (obs[i].r) > (dist - ERR_DIST) ){
            printf("On est dans un robot\n");
            return 1;
            }
        }

    return 0;
    }

int checkRobotBlock(void){
    static sPt_t pos[10] ={{0., 0.}};
    static int pt = 0;
    static unsigned int lastTime = 0;
    int i, cpt = 0;
    sNum_t dist;

    if( fabs(time_diff(millis(), lastTime)) > 200){
        pos[pt] = obs[0].c;
        pt++;
        pt = pt%10;
        for(i = 0 ; i < 10 ; i++){
            distPt2Pt(&obs[0].c, &pos[i], &dist);
            if(dist < 1.) cpt++;
            }
        if(cpt == 10){
            //printf("Warning robot block\n");
            return 1;
            }
        lastTime = millis();
        }

    return 0;
    }

void stopRobot(void){
    sPath_t path;
    sTrajEl_t traj;

    traj.p1 = obs[0].c;
    traj.p2 = obs[0].c;
    traj.arc_len = 0.;
    traj.seg_len = 0.;
    traj.sid = 0;

    path.path_len = 1;
    path.path = &traj;

    send_robot(path);
    }

//TODO Optimisation des deplacement du robot algarithme arbre recouvrant

