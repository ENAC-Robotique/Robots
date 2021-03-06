#ifndef LIB_GEOMETRYTOOLS_VECTOR3D_H_
#define LIB_GEOMETRYTOOLS_VECTOR3D_H_

#include <Vector2D.h>
#include <iostream>

#ifdef USE_OPENCV
#include <opencv2/core/core.hpp>
#endif

template<typename T>
class Vector3D;

template<typename T>
std::ostream& operator<<(std::ostream& out, const Vector3D<T>& v);

template<typename T>
class Vector3D {
protected:
    T _x, _y, _z;
    bool _norm;

    Vector3D(const T _x, const T _y, const T _z, const bool _norm) :
            _x(_x), _y(_y), _z(_z), _norm(_norm) {
    }

public:
    static const Vector3D zero;
    static const Vector3D xAxis;
    static const Vector3D yAxis;
    static const Vector3D zAxis;

    Vector3D() :
            _x(0), _y(0), _z(0), _norm(true) {
    }
    Vector3D(const Vector3D& v) :
            _x(v._x), _y(v._y), _z(v._z), _norm(v._norm) {
    }
    Vector3D(const T _x, const T _y, const T _z) :
            _x(_x), _y(_y), _z(_z), _norm(false) {
    }
    Vector3D(const Vector2D<T> v, T z) :
            _x(v._x), _y(v._y), _z(z), _norm(false) {
    }
#ifdef USE_OPENCV
    Vector3D(const cv::Mat& m) :
            _x(m.at<T>(0)), _y(m.at<T>(1)), _z(m.at<T>(2)), _norm(false) {
    }
#endif
    ~Vector3D() {
    }

    const T& x() const {
        return _x;
    }
    const T& y() const {
        return _y;
    }
    const T& z() const {
        return _z;
    }

    T operator*(const Vector3D& v) const { // Dot product
        return _x * v._x + _y * v._y + _z * v._z;
    }
    Vector3D<T> operator^(const Vector3D& v) const { // Cross product
        return Vector3D((_y * v._z - _z * v._y), (-_x * v._z + _z * v._x), (_x * v._y - _y * v._x), _norm && v._norm);
    }

    Vector3D operator+(const Vector3D& v) const {
        return {_x + v._x, _y + v._y, _z + v._z, false};
    }
    Vector3D operator-(const Vector3D& v) const {
        return {_x - v._x, _y - v._y, _z - v._z, false};
    }
    Vector3D operator*(const T& r) const {
        return {_x*r, _y*r, _z*r, false};
    }
    Vector3D operator/(const T& r) const {
        return {_x/r, _y/r, _z/r, false};
    }

    Vector3D& operator+=(const Vector3D& v) {
        return *this = *this + v;
    }
    Vector3D& operator-=(const Vector3D& v) {
        return *this = *this - v;
    }
    Vector3D& operator*=(const T& r) {
        return *this = *this * r;
    }
    Vector3D& operator/=(const T& r) {
        return *this = *this / r;
    }
    Vector3D& operator=(const Vector3D& v) {
        _x = v._x;
        _y = v._y;
        _z = v._z;
        _norm = v._norm;
        return *this;
    }

    bool operator==(const Vector3D& v) const {
        return v._x == _x && v._y == _y && v._z == _z;
    }
    bool operator!=(const Vector3D& v) const {
        return !(*this == v);
    }

    T norm() const {
        return _norm ? T(1) : std::sqrt(_x * _x + _y * _y + _z * _z);
    }
    T normSq() const {
        return _norm ? T(1) : _x * _x + _y * _y + _z - _z;
    }
    Vector3D<T>& normalize() {
        if (!_norm && (_x || _y || _z)) {
            T n = norm();
            _x /= n;
            _y /= n;
            _z /= n;
        }
        _norm = true;
        return *this;
    }
//    void rotate(const T& theta) {  // TODO
//    }
    T angle(const Vector3D& v) {
        return std::acos(*this * v / std::sqrt(normSq() * v.normSq()));
    }

    bool isZero(T eps) {
        return (std::abs(_x) <= eps) && (std::abs(_y) <= eps) && (std::abs(_z) <= eps);
    }

#ifdef USE_OPENCV
    cv::Mat toCv() const {
        return (cv::Mat_<T>(3, 1) << _x, _y, _z);
    }
#endif

    friend std::ostream& operator<<<T>(std::ostream& out, const Vector3D& v);
};

template<typename T>
Vector3D<T> operator*(const T& n, const Vector3D<T>& v) {
    return v * n;
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const Vector3D<T>& v) {
    out << "(" << v._x << ";" << v._y << ";" << v._z << ")";
    return out;
}

template<typename T>
const Vector3D<T> Vector3D<T>::zero { 0, 0, 0, true };
template<typename T>
const Vector3D<T> Vector3D<T>::xAxis { 1, 0, 0, true };
template<typename T>
const Vector3D<T> Vector3D<T>::yAxis { 0, 1, 0, true };
template<typename T>
const Vector3D<T> Vector3D<T>::zAxis { 0, 0, 1, true };

#endif /* LIB_GEOMETRYTOOLS_VECTOR3D_H_ */
