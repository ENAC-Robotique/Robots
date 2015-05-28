/*
 * clap.hpp
 *
 *  Created on: 30 janv. 2015
 *      Author: seb
 */

#ifndef OBJ_SPOT_H_
#define OBJ_SPOT_H_


#include <types.h>
#include "obj.h"
#include "tools.h"

#define START_STAND 9 //number of the first stand element in obs[]

typedef enum {
    SPOT_TRAJ1,
    SPOT_WAIT_TRAJ1,
    SPOT_GET_STAND1,
    SPOT_END
} stepSpot;

using namespace std;

class Spot : public Obj{
    public:
        Spot(const unsigned int num, eColor_t color, vector<astar::sObs_t>& obs);
        virtual ~Spot();

        void initObj(paramObj)  {};
        int loopObj(paramObj) ;
        eObj_t type() const  {return E_SPOT;};


        int updateDestPointOrient(paramObj par){
            unsigned int i;

            if(par.act.empty())
                return -1;

            for(i = 0 ; i < par.act.size() ; i++){ //TODO optimize for the moment the first find is used
                if( par.act[i].type == _typeAct){
                    if((!par.act[i].elevator.full))
                        break;
                }
            }

            if(i == par.act.size()){
                _actuator_select = -1;
                return -1;
            }

             _access_select_angle += par.act[i].angle;
             _actuator_select = par.act[i].id;

            return 0;
        }

    private :
        unsigned int _num;
        eColor_t _color;
        stepSpot stepLoc;
        unsigned int timePrev;
        Point2D<float> destPoint;

};

#endif /* OBJ_SPOT_H_ */
