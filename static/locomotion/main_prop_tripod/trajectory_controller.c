/*
 * controller_trajectory.c
 *
 *  Created on: 11 févr. 2015
 *      Authors: Ludovic Lacoste, yoyo
 */

#include <mt_mat.h>
#include <param.h>
#include <stdlib.h>
#include <string.h>
#include <tools.h>
#include <trajectory_controller.h>

#if NB_PODS != 3
#error "You can't change NB_PODS without changing trajectory_controller.c as well!"
#endif

void trajctl_init(trajectory_controller_t* ctl, int32_t mat_base[NB_SPDS][NB_PODS]) {
    int i, j;
    memset(ctl, 0, sizeof(*ctl));

    // Transformation matrices
    mt_m_init(&ctl->mat_sp_pods2rob, NB_SPDS, NB_PODS);
    mt_m_init(&ctl->mat_sp_rob2pods, NB_PODS, NB_SPDS);
    for (i = 0; i < NB_SPDS; i++) {
        for (j = 0; j < NB_PODS; j++) {
            MT_M_AT(&ctl->mat_sp_pods2rob, i, j)= mat_base[i][j];
        }
    }
#if NB_PODS == NB_SPDS
    mt_m_inv(&ctl->mat_sp_pods2rob, &ctl->mat_sp_rob2pods);
#else
#error "Case where NB_PODS != NB_SPDS is not yet implemented"
#endif

    // Init encoders, motors and speed controllers
    encoders_init(ctl->encs);
    motors_init(ctl->mots);
    for (i = 0; i < NB_PODS; i++) {
        spdctl_init(&ctl->spd_ctls[i], &ctl->encs[i]);
    }

    // Init PID
    pid_init(&ctl->pid_traj, 1, 0, 0, 0, 0); // TODO find good values
    pid_init(&ctl->pid_orien, 1, 0, 0, 0, 0); // TODO find good values
}

void _update_pos_orien(trajectory_controller_t* ctl, MT_VEC *spd_pv_rob);
void _trajectory_control(trajectory_controller_t* ctl, int x_sp, int y_sp, MT_VEC* spd_cmd_rob);
void _orientation_control(trajectory_controller_t* ctl, int o_sp, MT_VEC* spd_cmd_rob);

void trajctl_update(trajectory_controller_t* ctl /* ,trajectory_sp(t), orientation_sp(t) */) {
    int i;
    MT_VEC spd_pv_pods = MT_V_INITS(NB_PODS); // (V1_pv, V2_pv, V3_pv)
    MT_VEC spd_pv_rob = MT_V_INITS(NB_SPDS); // (Vx_pv, Vy_pv, Oz_pv)
    int x_sp, y_sp, o_sp;
    MT_VEC spd_cmd_rob = MT_V_INITS(NB_SPDS); // (Vx_cmd, Vy_cmd, Oz_pv)
    MT_VEC spd_cmd_pods = MT_V_INITS(NB_PODS); // (V1_cmd, V2_cmd, V3_cmd)

    // Update ctl from trajectory_sp(t), orientation_sp(t)
    // TODO update x_sp, y_sp, o_sp

    // Gets process value from the three encoders
    // => V1_pv, V2_pv, V3_pv and backups the nbticks
    for (i = 0; i < NB_PODS; i++) {
        encoder_update(&ctl->encs[i]);
        spd_pv_pods.ve[i] = encoder_get(&ctl->encs[i]);
    }

    // Send previously computed command to the motors
    for (i = 0; i < NB_PODS; i++) {
        motor_update(&ctl->mots[i], ctl->next_spd_cmds[i]);
    }

    // Get the speeds transformed in the robot's reference frame
    // (V1_pv, V2_pv, V3_pv) => (Vx_pv, Vy_pv, Oz_pv)
    mt_mv_mlt(&ctl->mat_sp_pods2rob, &spd_pv_pods, &spd_pv_rob);

    // Compute new position and orientation from
    // (Vx_pv, Vy_pv, Oz_pv, x, y, o) => (x, y, o)
    _update_pos_orien(ctl, &spd_pv_rob);

    // Trajectory control
    //(some pid... ; gives speed orientation set point: Vx_cmd, Vy_cmd)
    _trajectory_control(ctl, x_sp, y_sp, &spd_cmd_rob);

    // Orientation control
    // (some pid as well... ; gives angular speed Oz_cmd)
    _orientation_control(ctl, o_sp, &spd_cmd_rob);

    // Calculate speed set points for each pod
    // (Vx_cmd, Vy_cmd and Oz_cmd) => (V1_cmd, V2_cmd, V3_cmd)
    mt_mv_mlt(&ctl->mat_sp_rob2pods, &spd_cmd_rob, &spd_cmd_pods);

    // call speed controller with V1_cmd, V2_cmd, V3_cmd
    for (i = 0; i < NB_PODS; i++) {
        ctl->next_spd_cmds[i] = spdctl_update(&ctl->spd_ctls[i], spd_cmd_pods.ve[i]);
    }
}

void _update_pos_orien(trajectory_controller_t* ctl, MT_VEC* spd_pv_rob) {
    // Compute the new position
    ctl->x += spd_pv_rob->ve[0];
    ctl->y += spd_pv_rob->ve[1];
    ctl->o += spd_pv_rob->ve[2]; // FIXME, make a modulo
}

void _trajectory_control(trajectory_controller_t* ctl, int x_sp, int y_sp, MT_VEC* spd_cmd_rob) {
    int errx, erry;

    // Limit acceleration (keeping ratio of errors constant)
    errx = x_sp - ctl->x;
    erry = y_sp - ctl->y;

    if (abs(errx) >= MAX_ACC) {
        errx = SIGN(errx) * MAX_ACC;
        x_sp = ctl->x + errx;
    }

    if (abs(erry) >= MAX_ACC) {
        erry = SIGN(erry) * MAX_ACC;
        y_sp = ctl->y + erry;
    }

    // Compute the speeds command Vx_cmd, Vy_cmd
    spd_cmd_rob->ve[0] = pid_update(&ctl->pid_traj, x_sp, ctl->x);
    spd_cmd_rob->ve[1] = pid_update(&ctl->pid_traj, y_sp, ctl->y);
}

void _orientation_control(trajectory_controller_t* ctl, int o_sp, MT_VEC* spd_cmd_rob) {
    int erro;

    // Limit angular acceleration
    erro = o_sp - ctl->o;

    if (abs(erro) >= MAX_ANG_ACC) {
        erro = SIGN(erro) * MAX_ANG_ACC;
        o_sp = ctl->o + erro;
    }

    // Compute the angular speed command Omega_cmd
    spd_cmd_rob->ve[2] = pid_update(&ctl->pid_orien, o_sp, ctl->o);
}
