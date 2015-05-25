/*
 * pos_uncertainty.c
 *
 *  Created on: 6 oct. 2014
 *      Author: ludo6431
 */

#include <assert.h>
#include <math.h>
#include <messages-elements.h>
#include <messages-position.h>

#define POS_UNCERTAINTY_INTERNALS
#include "pos_uncertainty.h"

#define MAX(a, b) ((a)>(b)?(a):(b))
#define MIN(a, b) ((a)>(b)?(b):(a))
#define CLAMP(m, v, M) MAX((m), MIN((v), (M)))

void pos_uncertainty_step_update(sGenericPosStatus *prev, sGenericPosStatus *next){
    // TODO
}

// see static/locomotion/simu for design files

void varxya2abc(float var_x, float var_y, float ca, float sa, s2DPUncert_internal *o){
// Converts rotated 2D gaussian to quadratic form coefficients
//   input:  f(x, y) = X²/(2*var_x) + Y²/(2*var_y)
//               where: X =  cos(an)*x + sin(an)*y
//                      Y = -sin(an)*x + cos(an)*y
//               where: cos(an) = ca
//                      sin(an) = sa
//   output: f(x, y) = ax² + 2bxy + cy²

    var_x = CLAMP(MINVARIANCE, var_x, MAXVARIANCE);
    var_y = CLAMP(MINVARIANCE, var_y, MAXVARIANCE);

    o->a = ca*ca/(2*var_x) + sa*sa/(2*var_y);
    o->b = ca*sa*(1/var_x - 1/var_y)/2;
    o->c = sa*sa/(2*var_x) + ca*ca/(2*var_y);
}

void abc2varxya(s2DPUncert_internal *i, float *var_x, float *var_y, float *an){
// Converts quadratic form coefficients to rotated 2D gaussian
//   input:  f(x, y) = ax² + 2bxy + cy²
//   output: f(x, y) = X²/(2*var_x) + Y²/(2*var_y)
//               where: X =  cos(an)*x + sin(an)*y
//                      Y = -sin(an)*x + cos(an)*y

    float k = 4*(i->a*i->c - i->b*i->b); // 1/(var_x*var_y)
    float tmp_sqrt = sqrtf((i->a - i->c)*(i->a - i->c) + 4*i->b*i->b);
    int noangle = 0;

    if(i->b == 0.f || 2*i->b > tmp_sqrt || 2*i->b < -tmp_sqrt){
        noangle = 1;
    }

    if(i->c < i->a){
        tmp_sqrt = - tmp_sqrt;
    }
    *var_x = (i->a + i->c + tmp_sqrt)/k;
    *var_y = (i->a + i->c - tmp_sqrt)/k;

    if(noangle){
        *an = 0.f;
    }
    else{
        float sin_2a = 2*i->b/tmp_sqrt;
        *an = -asinf(CLAMP(-1.f, sin_2a, 1.f))/2;
    }
}

void gstatus2internal(sGenericPosStatus *i, s2DPUncert_internal *o){
    varxya2abc(i->pos_u.a_var, i->pos_u.b_var, cosf(i->pos_u.a_angle), sinf(i->pos_u.a_angle), o);
    o->x = i->pos.x;
    o->y = i->pos.y;
}

void internal2gstatus(s2DPUncert_internal *i, sGenericPosStatus *o){
    abc2varxya(i, &o->pos_u.a_var, &o->pos_u.b_var, &o->pos_u.a_angle);
    o->pos.x = i->x;
    o->pos.y = i->y;
}

void pos_uncertainty_mix(sGenericPosStatus *i1, sGenericPosStatus *i2, sGenericPosStatus *o){
    s2DPUncert_internal pg, mn, nw;

    // necessary verifications
    assert(i1 && i2 && o);
    assert(i1->pos.frame == FRAME_PLAYGROUND);
    assert(i1->pos.frame == i2->pos.frame);
    assert(i1->id == i2->id);
    assert(i1->date == i2->date);

    // get input data
    gstatus2internal(i1, &pg);
    gstatus2internal(i2, &mn);

    // actual calculation
    nw.a = pg.a + mn.a;
    nw.b = pg.b + mn.b;
    nw.c = pg.c + mn.c;

    float den = nw.a*nw.c - nw.b*nw.b;
    float G = -(mn.a*(mn.x - pg.x) + mn.b*(mn.y - pg.y));
    float H = -(mn.b*(mn.x - pg.x) + mn.c*(mn.y - pg.y));
    nw.x = (H*nw.b - G*nw.c) / den + pg.x;
    nw.y = (G*nw.b - H*nw.a) / den + pg.y;

    // fill output
    o->id = i1->id;
    o->date = i1->date;
    internal2gstatus(&nw, o);
    o->pos.frame = FRAME_PLAYGROUND;
}

s2DPAProbability pos_uncertainty_eval(sGenericPosStatus *i, s2DPosAtt *p){
    assert(i->pos.frame == p->frame);

    s2DPUncert_internal ii;
    gstatus2internal(i, &ii);

    s2DPAProbability o;
    float x = p->x - ii.x;
    float y = p->y - ii.y;
    o.xy_probability = ii.a*x*x + 2*ii.b*x*y + ii.c*y*y;
    o.theta_probability = 0.f;
    return o;
}
