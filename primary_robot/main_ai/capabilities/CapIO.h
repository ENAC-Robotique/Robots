/*
 * CapIOProcess.h
 *
 *  Created on: 12 avr. 2015
 *      Author: Sébastien Malissard
 */

#ifndef CAPABILITIES_CAPIO_H_
#define CAPABILITIES_CAPIO_H_

#include <Capability.h>
#include "CapTeam.h"

class CapIO : public Capability {
    public:
        CapIO(Robot* robot_init) : Capability(robot_init){};
        virtual ~CapIO(){};

// TODO setServo, getServo, getHMI, setHMI

        virtual unsigned int getHMI(const eIhmElement& el){
            return ihm.getValue(el);
        }

        virtual void setHMI(const eIhmElement& el, const unsigned int& value){
            ihm.sendIhm(el,value);
        }

        virtual void selectColor() {
            static int state = 0;
            CapTeam* capTeam = dynamic_cast<CapTeam*> (robot->caps[eCap::TEAM]);

            switch(state) {
                case 0 :
                    if(getHMI(IHM_BUTTON) == 0) {
                        state = 1;
                        capTeam->setColor(eColor_t::GREEN);
                        setHMI(IHM_LED, LED_GREEN);
                    }
                    break;
                case 1 :
                    if(getHMI(IHM_BUTTON) == 1)
                        state = 2;
                    break;
                case 2 :
                    if(getHMI(IHM_BUTTON) == 0) {
                        state = 3;
                        capTeam->setColor(eColor_t::PURPLE);
                        setHMI(IHM_LED, LED_PURPLE);
                    }
                    break;
                case 3 :
                    if(getHMI(IHM_BUTTON) == 1)
                        state = 0;
                break;
            }
        }
};


#endif /* CAPABILITIES_CAPIO_H_ */
