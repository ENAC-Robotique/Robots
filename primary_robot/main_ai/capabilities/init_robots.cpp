/*
 * init_robots.cpp
 *
 *  Created on: 30 mars 2015
 *      Author: Sébastien Malissard
 */

#include <Env.h>
#include <vector>

#include "init_robots.h"

#include "network_cfg.h"

#include "Robot.h"
#include "CapPosition.h"
#include "CapPosSimuSecondary.h"
#include "CapPosSimuAdversary.h"
#include "CapPreparation.h"
#include "CapPrepPrimary.h"
#include "CapIOSimuPrimary.h"
#include "CapPosStatuses.h"
#include "CapPropulsion.h"
#include "CapPropAxle.h"
#include "CapPropHolonome.h"
#include "CapSlave.h"
#include "CapAI.h"
#include "CapIO.h"
#include "CapActuator.h"
#include "environment.h"


std::vector<Robot*> robots;

using namespace Env2016;

void setupRobots(bool primary_prop_simu, bool primary_prop_holo, bool primary_hmi_simu, eColor_t primary_color, eAIState_t eState){

    // Primary
    bn_Address primary_addr_prop = primary_prop_simu?ADDRD1_MAIN_PROP_SIMU:ADDRI_MAIN_PROP;

    robots.push_back(new Robot("", ELT_PRIMARY, &env));

    robots.back()->caps[eCap::POS] = new CapPosStatuses(robots.back(), 0);
    robots.back()->caps[eCap::ACTUATOR] = new CapActuator(robots.back());
    if(eState == E_AI_SLAVE)
        robots.back()->caps[eCap::SLAVE] = new CapSlave(robots.back());
    else{ //PRIM_MODE_AI
        robots.back()->caps[eCap::TEAM] = new CapTeam(robots.back(), primary_color);
        robots.back()->caps[eCap::PREP] = new CapPrepPrimary(robots.back());
        robots.back()->caps[eCap::AI] = new CapAI(robots.back());
        if(primary_hmi_simu)
              robots.back()->caps[eCap::IO] = new CapIOSimuPrimary(robots.back());
        else
              robots.back()->caps[eCap::IO] = new CapIO(robots.back());
    }
    if(primary_prop_holo)
        robots.back()->caps[eCap::PROP] = new CapPropHolonome(robots.back(), primary_addr_prop);
    else
        robots.back()->caps[eCap::PROP] = new CapPropAxle(robots.back(), primary_addr_prop);

    // Secondary
    if(eState == E_AI_AUTO){
/*
        robots.push_back(new Robot("", ELT_SECONDARY, &env));

        robots.back()->caps[eCap::TEAM] = new CapTeam(robots.back(), primary_color);
        robots.back()->caps[eCap::POS] = new CapPosSimuSecondary(robots.back(), 1);
*/

        robots.push_back(new Robot("", ELT_ADV_PRIMARY, &env));

        robots.back()->caps[eCap::TEAM] = new CapTeam(robots.back(), primary_color==eColor_t::PURPLE?eColor_t::GREEN:eColor_t::PURPLE);
        robots.back()->caps[eCap::POS] = new CapPosStatuses(robots.back(), 2);

    }

    // Call setup method
    for(Robot* r : robots){
        if(CapSlave* capSlave = dynamic_cast<CapSlave*> (r->caps[eCap::SLAVE])){
            capSlave->setup();
        }
        if(CapActuator* capActuator = dynamic_cast<CapActuator*> (r->caps[eCap::ACTUATOR])){
            capActuator->setup();
        }
    }

}


int loopRobots(){ //TODO end of game
    static bool game = false;

    if(!game){
        unsigned int i = 0;
        for(Robot* r : robots){
            if(CapPreparation* capPrep = dynamic_cast<CapPreparation*> (r->caps[eCap::PREP])){
                if(capPrep->loop() == 1)
                    game = true;
            }
            else
                i++;
        }
        if(i == robots.size())
            game = true;
    }
    else{
        for(Robot* r : robots){
            Point2D<float> pos(0,0);
            if(CapPosition* capPos = dynamic_cast<CapPosition*> (r->caps[eCap::POS])){
                Point2D<float> prevPos(r->env->obs[capPos->getIobs()].c.x, r->env->obs[capPos->getIobs()].c.y);
                pos = capPos->getLastPosXY();
                if(prevPos != pos){
                    r->env->obs[capPos->getIobs()].c = {pos.x, pos.y};
                    r->env->obs_updated[capPos->getIobs()]++;
                }
            }

            if(pos.x != 0 && pos.y != 0){
                if(CapAI* capAI = dynamic_cast<CapAI*> (r->caps[eCap::AI])){
                    if(!capAI->loop())
                        return 0;
                }
                if(CapSlave* capSlave = dynamic_cast<CapSlave*> (r->caps[eCap::SLAVE])){
                    capSlave->loop();
                }
            }
        }
    }

    return 1;
}
