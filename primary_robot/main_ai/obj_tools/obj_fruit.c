/*
 * obj_fruit.c
 *
 *  Created on: 17 avr. 2014
 *      Author: seb
 */

#include "obj_fruit.h"

#include "obj_com.h"


//Definition of all different trajectory we need for collect the fruits in the trees

	sTrajEl_t tabEl[4]={ //Trajectory for tree upwards vertical in the origin of the table for tree
		{{0.  ,  0.},{15.2,-16.9},{{33.2 ,-16.9}, 18. , 0., 1.}, 0. , 0., 0},
		{{17.3,-8.5},{17.3, -8.5},{{-4.9 ,  3.4},-25.2, 0., 1.}, 0. , 0., 1},
		{{17.3,15.2},{17.3, 15.2},{{33.3 , 23.7}, 18.1, 0., 1.}, 0. , 0., 2},
		{{15.2,23.7},{15.2, 23.7},{{15.2 , 23.7}, 0.  , 0., 1.}, 0. , 0., 3}
		};

	sTrajEl_t tabEl2[3]={ //Trajectory for tree upwards vertical in the origin of the table for tree with a bad fruitmouth
		{{ 0. , 0. },{20.3,  3.4},{{-4.9,  3.4},-25.2, 0., 1.}, 0. , 0., 0},
		{{17.3,15.2},{17.3, 15.2},{{33.3, 23.7}, 18.1, 0., 1.}, 0. , 0., 1},
		{{15.2,23.7},{15.2, 23.7},{{15.2, 23.7},  0. , 0., 1.}, 0. , 0., 2}
		};

	sTrajEl_t tabEl3[4]={ //Trajectory for tree upwards vertical in the origin of the table for tree with a bad fruitmouth
		{{0.  ,  0.},{15.2,-16.9},{{33.2 ,-16.9}, 18. , 0., 1.}, 0. , 0., 0},
		{{17.3,-8.5},{17.3, -8.5},{{-4.9 ,  3.4},-25.2, 0., 1.}, 0. , 0., 1},
		{{20.3, 3.4},{20.3, 23.7},{{15.2 , 23.7}, 0.  , 0., 1.}, 0. , 0., 2},
		{{20.3,23.7},{20.3, 23.7},{{15.2 , 23.7}, 0.  , 0., 1.}, 0. , 0., 3}
		};

	sTrajEl_t tabEl4[2]={
        {{0.  ,  0.},{23. ,29.},{{0. , 0.}, 0. , 0., 1.}, 0. , 0., 0},
        {{23. , 29.},{23., 29.},{{0. , 0.}, 0. , 0., 1.}, 0. , 0., 1}
	    };

// Defintion of the entry point of the trajectory for a tree in (0;0) vertical bottom to top right

	sPt_t tabEP[3]={
		{16, -27},
		{20.3, -27},
		{23, -30}
		};



//Function for the entry point to a tree

	int PATree(int num){ //return the code of the state tree (location bad fruit)
	    if( (listObj[num].utype.tree.eFruit[0] == 2) && (listObj[num].utype.tree.eFruit[0] == 2) ) return 21; //FIXME ??
		if(listObj[num].utype.tree.eFruit[0] == 2) return 31;
		if(listObj[num].utype.tree.eFruit[3] == 2) return 20;
		else return 30;
		}

	void updateEntryPointTree(void){
		int num, id, j;
		sPt_t tabTemp;

		for( num=0 ; num<4 ; num++){
			id=PATree(num);
			for( j=0 ; j<2 ; j++){
				switch(id){
					case 30 :
						tabTemp=tabEP[0];
						break;
					case 20 :
						if(j==1) tabTemp=tabEP[1];
						else tabTemp=tabEP[0];
						break;
					case 31 :
						if(j==1) tabTemp=tabEP[0];
						else tabTemp=tabEP[1];
						break;
					case 21 :
					    tabTemp=tabEP[2];
					    break;
					default :
						printf("Error in switch in updateEntryPointTree\n");
						getchar();
				}
				if(j==0) SymPt(&tabTemp, 1, 0);
				switch(num){
					case 0 :
						TransPt(&tabTemp, listObj[num].utype.tree.x, listObj[num].utype.tree.y);
						break;
					case 1 :
					case 2 :
						Rot90Pt(&tabTemp);
						TransPt(&tabTemp, listObj[num].utype.tree.x, listObj[num].utype.tree.y);
						break;
					case 3 :
						SymPt(&tabTemp, 1, 1);
						TransPt(&tabTemp, listObj[num].utype.tree.x, listObj[num].utype.tree.y);
						break;
					}
				//printf("y: %d", listObj[num].utype.tree.y);
				//getchar();
				listObj[num].entryPoint[j].c=tabTemp;
				}
			}
		}


//Function for find the good trajectory

int getEntryPointTree(sPt_t *pt){ //Ex return 124, 431..
	int i, j;

	for( i=0 ; i<4 ; i++){
		for( j=0 ; j<2 ; j++){
			if(listObj[i].entryPoint[j].c.x == pt->x && listObj[i].entryPoint[j].c.y == pt->y){
				if(j==0){
				    if(listObj[i].utype.tree.eFruit[0]==2 && listObj[i].utype.tree.eFruit[3]==2){
				        return i*100+1*10+2;
				        }
					if(listObj[i].utype.tree.eFruit[0]==2) return i*100+1*10+3;
					else {
						if(listObj[i].utype.tree.eFruit[3]==2) return i*100+0*10+2;
						else return i*100+0*10+3;
						}
					}
				else{
				    if(listObj[i].utype.tree.eFruit[0]==2 && listObj[i].utype.tree.eFruit[3]==2){
				        return i*100+2*10+1;
				    }
					if(listObj[i].utype.tree.eFruit[3]==2) return i*100+2*10+0;
					else {
						if(listObj[i].utype.tree.eFruit[0]==2) return i*100+3*10+1;
						else return i*100+3*10+0;
						}
					}
				}
			}
		}
	return -1;
	}

int sendInitTrajTree(iABObs_t obj){
    int i, idEntryTree, numTree, idEntry;
    sTrajEl_t *tabTemp = NULL;

	idEntryTree=getEntryPointTree(&pt_select);

	numTree=idEntryTree/100;
	if(numTree != obj) {
		printf("Error : numTree != obj\n");
		getchar();
		}
	idEntry=idEntryTree%100;

    checkFire(obj, idEntryTree);

	switch(idEntry){
		case 30 :
		case 03 :
			basket.nb_point = basket.nb_point + 4; //FIXME si le robot ne peut pas terminer l'arbre
			path.path_len = 4;
			if ((tabTemp = (sTrajEl_t *) malloc(sizeof(tabEl[0])*path.path_len)) == NULL)
				printf("Error : malloc()\n");
			memcpy(tabTemp,&tabEl[0], sizeof(tabEl[0])*path.path_len);
			if(idEntry==03){
				for(i=0; i<path.path_len ; i++) SymElTraj(tabTemp+i, 1, 0);
				}
			break;
		case 20 :
		case 13 :
			basket.nb_point = basket.nb_point + 3;
			path.path_len = 3;
			if ((tabTemp = (sTrajEl_t *) malloc(sizeof(tabEl2[0])*path.path_len)) == NULL)
				printf("Error : malloc()\n");
			memcpy(tabTemp,&tabEl2[0], sizeof(tabEl2[0])*path.path_len);
			if(idEntry==13){
				for(i=0; i<path.path_len ; i++) SymElTraj(tabTemp+i, 1, 0);
				}
			break;
		case 31 :
		case 02 :
			basket.nb_point = basket.nb_point + 3;
			path.path_len=4;
			if ((tabTemp = (sTrajEl_t *) malloc(sizeof(tabEl3[0])*path.path_len)) == NULL)
				printf("Error : malloc()\n");
			memcpy(tabTemp,&tabEl3[0], sizeof(tabEl3[0])*path.path_len);
			if(idEntry==02){
				for(i=0; i<path.path_len ; i++) SymElTraj(tabTemp+i, 1, 0);
				}
			break;
		case 12 :
		case 21 :
		    basket.nb_point = basket.nb_point + 3;
		    path.path_len = 2;
            if ((tabTemp = (sTrajEl_t *) malloc(sizeof(tabEl4[0])*path.path_len)) == NULL)
                printf("Error : malloc()\n");
            memcpy(tabTemp,&tabEl4[0], sizeof(tabEl4[0])*path.path_len);
            if(idEntry==12){
                for(i=0; i<path.path_len ; i++) SymElTraj(tabTemp+i, 1, 0);
                }
            break;
		default :
			printf("Error in switch in obj_tree\n");
			getchar();
		}

	for(i=0; i<path.path_len ; i++){
		switch(numTree){
			case 0 :
				TransElTraj(tabTemp+i, listObj[obj].utype.tree.x, listObj[obj].utype.tree.y);
				break;
			case 1 :
			case 2 :
				Rot90Traj(tabTemp+i);
				TransElTraj(tabTemp+i, listObj[obj].utype.tree.x, listObj[obj].utype.tree.y);
				break;
			case 3 :
				SymElTraj(tabTemp+i, 1, 1);
				TransElTraj(tabTemp+i, listObj[obj].utype.tree.x, listObj[obj].utype.tree.y);
				break;
			}
		}

	memcpy(&pt_select, &tabTemp[path.path_len-1].p2, sizeof(pt_select));
	tabTemp[0].p1=obs[0].c;
	path_len(tabTemp, 4);
	path.path=tabTemp;
	send_robot(path) ;

	free(tabTemp);
    return idEntryTree;
	}


void  checkFire(int num_obj, int idEntryTree){
    int i;
    sNum_t dist;
    printf("Start checkFire\n");
    for( i = 0 ; i < NB_OBJ ; i++){
        if( (listObj[i].etype == E_FEU) && (listObj[i].utype.fire.pos == 3) ){
            distPt2Pt(&obs[listObj[i].numObs[0]].c, &obs[listObj[num_obj].numObs[0]].c, &dist);
            if( dist < 50. ){
                printf("inf 50 checkFire : %d\n", (idEntryTree/10)%10);
               switch((idEntryTree/10)%10){
                   case 0 :
                   case 1 :
                       if( listObj[i].utype.fire.angle == listObj[num_obj].utype.tree.angle){
                           if(color == 1){
                               printf("Active bras gauche\n");
                               //TODO actionner bras gauche
                               }
                           }
                       printf("feu : %f, arbre :%f\n", listObj[i].utype.fire.angle +180, listObj[num_obj].utype.tree.angle);
                       if( fmodf(listObj[i].utype.fire.angle +180,360.) == listObj[num_obj].utype.tree.angle){
                           if(color == 0){
                               printf("Active bras gauche\n");
                               //TODO actionner bras gauche
                               }
                           }
                       break;
                   case 2 :
                   case 3 :
                       if( listObj[i].utype.fire.angle == listObj[num_obj].utype.tree.angle){
                           if(color == 0){
                               printf("Active bras droit\n");
                               //TODO actionner bras droit
                               }
                           }
                       if( fmodf(listObj[i].utype.fire.angle +180,360.) == listObj[num_obj].utype.tree.angle){
                           if(color == 1){
                               printf("Active bras droit\n");
                               //TODO actionner bras droit
                               }
                           }
                       break;

               }

                }
            }
        }
    }


//standard function objective

void obj_tree(iABObs_t obj)
	{
	static int state=0; //for separate the init, loop and end
	int i, num_obs_in;

	switch(state){
		case 0:
			printf("Debut objectif arbre\n");
			for(i=0; i<4 ; i++){  //disable entry point
				obs[N-i-2].active=0;
				obs_updated[N-i-2]++;
				}

			//TODO Temporaire a afiner en fonction si l'objectif est bientot terminer
			listObj[obj].active=0;

			sendInitTrajTree(obj);

			obs[N-1].c=pt_select;
			obs_updated[N-1]++;

			obs[listObj[obj].numObs[0]].r = R_ROBOT + 5;
			obs_updated[listObj[obj].numObs[0]]++;

			//generique fonction
			last_time=millis();
			mode_obj=1;
			state=1;

			printf("start x=%f y=%f end x=%f y=%f\n",obs[0].c.x,obs[0].c.y,obs[N-1].c.x,obs[N-1].c.y);
			break;
		case 1:
		    if( checkAdvOnRobot() ){
		        stopRobot();
		        mode_obj=0;
		        }
			if ((fabs(pt_select.x-_current_pos.x)<1 && fabs(pt_select.y-_current_pos.y)<1)){
				state=2;
				}
			break;
		case 2 :
			mode_obj=0;
			state=0;
			pt_select.x=0;
			pt_select.y=0;
			listObj[obj].dist=0;
			//for no discontinuity of the end obj because of fire
			num_obs_in =  test_in_obs(&obs[0].c);
			if( (START_FEU <= num_obs_in) && (num_obs_in <= END_FEU) ){
			    obs[num_obs_in].active = 0; //FIXME
			    }
		   break;
		default :
			printf("Error in obj_tree : state != (0 || 1 || 2\n");
			break;
		}
	}


void obj_bac(iABObs_t obj){
    sVec_t v;
    sMsg msgOut;
    static int state = 0 ;

    switch(state){
        case 0 :
            listObj[obj].active=0;
            mode_obj=1;
            //obs[listObj[obj].numObs[0]].active=0;
            obs_updated[listObj[obj].numObs[0]]++;
            basket.nb_point=0;

            v.x = 0.;
            v.y = 20.;
            sendSeg(NULL, &v);

            pt_select.x = obs[0].c.x + v.x;
            pt_select.y = obs[0].c.y + v.y;

            state=1;

            break;
        case 1 :
            if ((fabs(pt_select.x-_current_pos.x)<1 && fabs(pt_select.y-_current_pos.y)<1)){
                state=2;
                }
            break;
        case 2 :
            msgOut.header.type = E_POS;
            msgOut.header.size = sizeof(msgOut.payload.pos);

            msgOut.payload.pos.id = 0;
            msgOut.payload.pos.u_a = 0;
            msgOut.payload.pos.u_a_theta = 0;
            msgOut.payload.pos.u_b = 0;
            msgOut.payload.pos.theta = theta_robot;
            msgOut.payload.pos.x = obs[0].c.x;
            msgOut.payload.pos.y = 157.;
            obs[0].c.y=157.;
            _current_pos.y = obs[0].c.y;
            printf("Sending initial position to robot%i (%.2fcm,%.2fcm,%.2f°).\n", msgOut.payload.pos.id, msgOut.payload.pos.x, msgOut.payload.pos.y, msgOut.payload.pos.theta*180./M_PI);

            role_send(&msgOut);

            newSpeed(-20.);
            v.x = 0.;
            v.y = -10.;
            sendSeg(NULL, &v);

            pt_select.x = obs[0].c.x + v.x;
            pt_select.y = obs[0].c.y + v.y;

            state = 3;
            break;
        case 3 :
            if ((fabs(pt_select.x-_current_pos.x)<1 && fabs(pt_select.y-_current_pos.y)<1)){
                state=4;
                }
            break;
        case 4 :
            newSpeed(20.);

            mode_obj=0;
            pt_select.x=0;
            pt_select.y=0;
            listObj[obj].dist=0;
            listObj[obj].active=0;
            state=0;
            #if DEBUG
                printf("Objectif : bac fini\n\n\n");
            #endif

            break;
        default:
            printf("Error : switch bac\n");
            getchar();
    }

    /*
    static int state=0; //for separate the init, loop and end
    int i;

    switch(state){
	    case 0:
            //generique fonction
    		last_time=millis();
    		mode_obj=1;
    		state=1;

            printf("start x=%f y=%f end x=%f y=%f\n",obs[0].c.x,obs[0].c.y,obs[N-1].c.x,obs[N-1].c.y);
	        break;
	    case 1:
	    	if( (millis()-last_time)>1000){
	            last_time=millis();
	            if ((fabs(pt_select.x-_current_pos.x)<1 && fabs(pt_select.y-_current_pos.y)<1)){
	            	state=2;
	            	}
	    		}
	    	break;
	    case 2 :
		    mode_obj=0;
		    state=0;
		    pt_select.x=0;
		    pt_select.y=0;
		    listObj[obj].dist=0;
		   break;
	    default :
	    	printf("Error in obj_fire : state != (0 || 1 || 2\n");
			break;
    	}
    */

    }


//standard function ratio

sNum_t ratio_arbre(void){
    //TODO décroissance lineaire
    float res;
    res=-(millis()-_start_time)/1000+90;
    return res;
    }

sNum_t ratio_bac(void){
    //TODO Croissance lineaire
    float res;
    res=(millis()-_start_time)/1000-20;
    return res;
    }
