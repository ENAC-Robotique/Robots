/*
 * environment.h
 *
 *  Created on: 19 avr. 2015
 *      Author: Sébastien Malissard
 */

#ifndef CAPABILITIES_ENVIRONMENT_2015_H_
#define CAPABILITIES_ENVIRONMENT_2015_H_

#include <Env.h>

/*
 * Creates an environment for up to four robots, developed for the "Coupe de France de Robotique 2015"
 */

#define BLOCK_START_ZONE    19

namespace Env2016{
    extern Environment env;

    void setup();
    int loop();

}



#endif /* CAPABILITIES_ENVIRONMENT_2015_H_ */
