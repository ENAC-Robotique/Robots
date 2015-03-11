/*
 * Segment2D.h
 *
 *  Created on: 8 mars 2015
 *      Author: Sébastien Malissard
 */

#ifndef LIB_GEOMETRYTOOLS_SEGMENT2D_H_
#define LIB_GEOMETRYTOOLS_SEGMENT2D_H_

#include "error.h"
#include "Point2D.h"
#include "Vector2D.h"

class Segment2D {
    public:
        Segment2D();
        Segment2D(Point2D _p1, Point2D _p2);
        ~Segment2D();

        ERROR convPts2Seg(const Point2D& a, const Point2D& b);
        ERROR sqdistPt2Seg(const Point2D& p, float& d, Point2D& h) const;

        Point2D p1;
        Point2D p2;
};

#endif /* LIB_GEOMETRYTOOLS_SEGMENT2D_H_ */
