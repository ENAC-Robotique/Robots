/*
 * segment.cpp
 *
 *  Created on: 8 mars 2015
 *      Author: Sébastien Malissard
 */

#include "segment.h"

Segment::Segment() : p1(0,0), p2(0,0){
}

Segment::Segment(Point _p1, Point _p2) : p1(_p1), p2(_p2){
}

Segment::~Segment() {
}

ERROR Segment::convPts2Seg(const Point& a, const Point& b) {

    p1 = a;
    p2 = b;

    return 0;
}

ERROR Segment::sqdistPt2Seg(const Point& p, float& d, Point& h) const{
    float l2, t;
    Vector p1p(p1, p), p1p2(p1, p2);

    p1.sqdistPt2Pt(p2, l2);
    if (l2 == 0.){
        return p.sqdistPt2Pt(p1, d);
    }

    p1p.dotVecs(p1p2, t);

    if (t < 0.){
        h = p1;
        return p.sqdistPt2Pt(p1, d);
    }

    if (t > l2) {
        h = p2;
        return p.sqdistPt2Pt(p2, d);
    }

    t = t / l2;

    h.x = p1.x + t * p1p2.x;
    h.y = p1.y + t * p1p2.y;

    return p.sqdistPt2Pt(h, d);
}
