/*
 * speed_controller.h
 *
 *  Created on: 11 févr. 2015
 */

#ifndef SPEED_CONTROLLER_H_
#define SPEED_CONTROLLER_H_

#include <encoder.h>
#include <motor.h>
#include <pid.h>

typedef struct {
    encoder_t* enc;
    motor_t* mot;

    PID_t pid;
} speed_controller_t;

void spdctl_init    (speed_controller_t* sc, encoder_t* enc, motor_t* mot);
void spdctl_update  (speed_controller_t* sc);

#endif /* SPEED_CONTROLLER_H_ */
