/*
 * Vector2D.h
 *
 *  Created on: 8 mars 2015
 *      Author: Sébastien Malissard
 */

#ifndef LIB_GEOMETRYTOOLS_VECTOR2D_H_
#define LIB_GEOMETRYTOOLS_VECTOR2D_H_

#include "error.h"
#include "Point2D.h"

template<typename T>
class Vector2D {
    public:
        Vector2D();
        Vector2D(T _x, T _y);
        Vector2D(const Point2D<T>& a, const Point2D<T>& b);
        ~Vector2D();

        ERROR normVec(T& n)const;
        ERROR convPts2Vec(const Point2D<T>& a, const Point2D<T>& b);
        ERROR dotVecs(const Vector2D& v, T& d)const;
        ERROR crossVecs(const Vector2D& v, T& c)const;
        ERROR rotVec(const T& theta);

        T x;
        T y;
};

#endif /* LIB_GEOMETRYTOOLS_VECTOR2D_H_ */
