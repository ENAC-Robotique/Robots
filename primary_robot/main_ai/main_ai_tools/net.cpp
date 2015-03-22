/*
 * net.cpp
 *
 *  Created on: 24 févr. 2015
 *      Author: seb
 */

#include <net.h>

#include <stdint.h>
#include <cmath>
#include <iostream>

extern "C"{
#include <unistd.h>
#include "roles.h"
}
#include "messages.h"
#include "tools.h"

#ifndef HOLONOMIC
#error "HOLONOMIC must be defined"
#endif

#define TRAJ_ORIENT 0 //1 active else 0

#define ROUND(a) ((int)((a) + 0.5))
#define POW2(b) ((double)(1 << (b)))
#define CONV2TRAJORIENT(a, b) ((int) ROUND(((double) (a)) * POW2(b)))
#define CONV2TRAJ(a, b) (((double) (a))/POW2(b))

unsigned int Net::_tid = 0; //the same tid is used by all path object

Net::Net() {
    // TODO Auto-generated constructor stub
}

Net::~Net() {
    // TODO Auto-generated destructor stub
}

void Net::maintenace(){

#if TRAJ_ORIENT || HOLONOMIC
    convTrajToTrajOrient();
    sendPathOrientToNet();
#else
    sendPathToNet();
#endif

}

/*
 * Clear _trajEl and _trajOrientEl
 */
void Net::clearEl(){//TODO warning if no empty
    queue <sTrajEl_t> empty;
    swap(_trajEl, empty);

    queue <sTrajOrientEl_t> emptyOrient;
    swap(_trajOrientEl, emptyOrient);
}

/*
 * Add a new path in the list of path ready to send
 */
void Net::sendPath(deque <sTrajEl_t> &trajEl){
    clearEl();

    for(const sTrajEl_t& i : trajEl){
        _trajEl.push(i);
    }
}

/*
 * Add a new path orient in the list of path ready to send
 */
void Net::sendPathOrient(deque <sTrajOrientEl_t> &trajElOrient){
    clearEl();

    for(const sTrajOrientEl_t& i : trajElOrient){
        _trajOrientEl.push(i);
    }
}

/*
 * Transfers _trajEl to _trajOrientEl and compute the new parameter.
 */
void Net::convTrajToTrajOrient(){
    sTrajOrientEl_t trajOrient;

    while(!_trajEl.empty()){
        trajOrient.t1 = 0; //TODO
        trajOrient.t2 = 0; //TODO

        trajOrient.p1 = _trajEl.front().p1;
        trajOrient.p2 = _trajEl.front().p2;

        trajOrient.obs = _trajEl.front().obs;

        trajOrient.theta1 = 0; //TODO get current theta
        trajOrient.theta2 = 0; //TODO get current theta

        trajOrient.seg_len = _trajEl.front().seg_len;
        trajOrient.arc_len = _trajEl.front().arc_len;

        trajOrient.rot1_dir = 0; //TODO
        trajOrient.rot2_dir  = 0; //TODO

        _trajEl.pop();
        _trajOrientEl.push(trajOrient);
    }

}

/*
 * Sends the path save in _trajEl to the prop with a E_TRAJ message
 */
void Net::sendPathToNet(){
    sMsg outMsg;
    int ret;

    // TODO do not do the message send here!!
    // This function must be not blocking
    // wait more between two messages

    if(!_trajEl.empty()){
        unsigned int size = _trajEl.size();

        outMsg.header.type = E_TRAJ;
        outMsg.header.size = sizeof(outMsg.payload.traj);

        outMsg.payload.traj.tid = _tid;

        for (unsigned int sid = 0; sid < size; sid++){
            outMsg.payload.traj.sid = sid;

            outMsg.payload.traj.p1_x = _trajEl.front().p1.x;
            outMsg.payload.traj.p1_y = _trajEl.front().p1.y;
            outMsg.payload.traj.p2_x = _trajEl.front().p2.x;
            outMsg.payload.traj.p2_y = _trajEl.front().p2.y;
            outMsg.payload.traj.seg_len = _trajEl.front().seg_len;

            outMsg.payload.traj.c_x = _trajEl.front().obs.c.x;
            outMsg.payload.traj.c_y = _trajEl.front().obs.c.y;
            outMsg.payload.traj.c_r = _trajEl.front().obs.r;
            outMsg.payload.traj.arc_len = _trajEl.front().arc_len;

            _trajEl.pop();

            if ((ret = role_sendRetry(&outMsg, MAX_RETRIES)) <= 0)
                logs << ERR << "role_sendRetry(E_TRAJ) failed #" << -ret;
            else
                logs << INFO <<"A new path was send : tid=" << _tid << " and sid=" << sid;

            usleep(1000);
        }

        _tid++;
    }
 }

/*
 * Sends the path orient save in _trajOrientEl to the prop with a E_TRAJ_ORIENT_EL message
 */
void Net::sendPathOrientToNet(){
    sMsg outMsg;
    int ret;

    // TODO do not do the message send here!!
    // This function must be not blocking
    // wait more between two messages

    if(!_trajOrientEl.empty()){
        outMsg.header.type = E_TRAJ_ORIENT_EL;
        outMsg.header.size = sizeof(outMsg.payload.trajOrientEl);

        for(unsigned int sid = 0 ; !_trajOrientEl.empty() ; sid++){
            for(unsigned int j = 0 ; j < 2 ; j++){
                const sTrajOrientEl_t& toe = _trajOrientEl.front();

                outMsg.payload.trajOrientEl.elts[j].p1_x = CONV2TRAJORIENT(toe.p1.x, 6);
                outMsg.payload.trajOrientEl.elts[j].p1_y = CONV2TRAJORIENT(toe.p1.y, 6);

                outMsg.payload.trajOrientEl.elts[j].p2_x = CONV2TRAJORIENT(toe.p2.x, 6);
                outMsg.payload.trajOrientEl.elts[j].p2_y = CONV2TRAJORIENT(toe.p2.y, 6);

                outMsg.payload.trajOrientEl.elts[j].c_x = CONV2TRAJORIENT(toe.obs.c.x, 6);
                outMsg.payload.trajOrientEl.elts[j].c_y = CONV2TRAJORIENT(toe.obs.c.y, 6);
                outMsg.payload.trajOrientEl.elts[j].c_r = CONV2TRAJORIENT(toe.obs.r, 5);

                outMsg.payload.trajOrientEl.elts[j].is_last_element = 0;

                outMsg.payload.trajOrientEl.elts[j].theta1 =  CONV2TRAJORIENT(toe.theta1, 13);
                outMsg.payload.trajOrientEl.elts[j].theta2 =  CONV2TRAJORIENT(toe.theta2, 13);

                outMsg.payload.trajOrientEl.elts[j].seg_len = CONV2TRAJORIENT(toe.seg_len, 5);
                outMsg.payload.trajOrientEl.elts[j].rot1_dir =  toe.rot1_dir;
                outMsg.payload.trajOrientEl.elts[j].arc_len = CONV2TRAJORIENT(toe.arc_len, 5);
                outMsg.payload.trajOrientEl.elts[j].rot2_dir = toe.rot2_dir;

                if(j == 0){
                    outMsg.payload.trajOrientEl.t   =  toe.t1;
                    outMsg.payload.trajOrientEl.dt1 = (toe.t2 - toe.t1)/1000;
                }else{
                    outMsg.payload.trajOrientEl.dt2 = (toe.t1 - outMsg.payload.trajOrientEl.t)/1000 - outMsg.payload.trajOrientEl.dt1;
                    outMsg.payload.trajOrientEl.dt3 = (toe.t2 - toe.t1)/1000;
                }

                _trajOrientEl.pop(); // toe becomes invalid, do not use after that
                if(_trajOrientEl.empty()){
                    outMsg.payload.trajOrientEl.elts[j].is_last_element = 1;
                    break;
                }
            }

            outMsg.payload.trajOrientEl.t += DELAY_MS * 1000; // add delay to compensate for message send duration
            outMsg.payload.trajOrientEl.tid = _tid;
            outMsg.payload.trajOrientEl.sid = sid;

            if ((ret = role_sendRetry(&outMsg, MAX_RETRIES)) <= 0)
                logs << ERR << "role_sendRetry(E_TRAJ_ORIENT_EL) failed #" << -ret;
            else
                logs << INFO << "A new path orient has been sent: tid=" << _tid << " and sid=" << sid;

            usleep(1000);
        }

        _tid++;
    }
}
