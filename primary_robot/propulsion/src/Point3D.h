/*
 * Point3D.h
 *
 *  Created on: 1 mai 2017
 *      Author: fabien
 */

#ifndef POINT3D_H_
#define POINT3D_H_

class Point3D {
public:
	Point3D();
	Point3D(double x, double y);
	Point3D(double x, double y, int speed);
	Point3D(double x, double y, double theta);
	Point3D(double x, double y, double theta, int speed);
	Point3D(double x, double y, int speed, int trajId, int pointId);
	virtual ~Point3D();

	double getTheta() const {
		return _theta;
	}

	void setTheta(double theta) {
		this->_careAboutTheta = true;
		this->_theta = theta;
	}

	double getX() const {
		return _x;
	}

	void setX(double x) {
		this->_x = x;
	}

	double getY() const {
		return _y;
	}

	void setY(double y) {
		this->_y = y;
	}



	bool careAboutTheta() const {
		return _careAboutTheta;
	}

	void setCareAboutTheta(bool careAboutTheta) {
		_careAboutTheta = careAboutTheta;
	}

	double getSpeed() const {
		return _speed;
	}

	int getPointId() const {
		return _pointId;
	}

	int getTrajId() const {
		return _trajId;
	}

	void setPointId(int pointId) {
		_pointId = pointId;
	}

	void setTrajId(int trajId) {
		_trajId = trajId;
	}

private:
	double _x;
	double _y;
	double _theta;
	double _speed;
	bool _careAboutTheta;
	int _trajId;
	int _pointId;
};

#endif /* POINT3D_H_ */
