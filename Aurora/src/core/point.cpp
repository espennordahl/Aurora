//
//  Point.cpp
//  Aurora
//


#include <iostream>
#include <assert.h>
#include <math.h>

#include "point.h"

#include "log.h"
#define lcontext LOG_Point

using namespace Aurora;

#pragma mark -
#pragma mark Initialization

Point::Point(float _x, float _y, float _z):
x(_x),
y(_y),
z(_z)
{
    assert(!hasNaNs());
}

Point::Point( float f ):
x(f),
y(f),
z(f)
{
    assert(!hasNaNs());
}

Point::Point(){
	Point(0,1,0);
}

Point::Point(const Vector &v):
x(v.x),
y(v.y),
z(v.z)
{
    assert(!hasNaNs());
}

bool Point::hasNaNs(){
	return (isnan(x) || isnan(y) || isnan(z));
}


#pragma mark -
#pragma mark Symbol Overloading

//	addition
Point Point::operator+(const Vector &v){
	return Point(x+v.x, y+v.y, z+v.z);
}

//Point Point::operator+(const Point &p){
//	return Point(x+p.x, y+p.y, z+p.z);
//}

Point &Point::operator+=(const Vector &v){
	x += v.x; y += v.y; z += v.z;
	return *this;
}

Point &Point::operator+=(const Point &p){
	x += p.x; y += p.y; z += p.z;
	return *this;
}


Point Point::operator+(const Point &p) const {
	return Point(x+p.x, y+p.y, z+p.z);
}

// subtraction
Point Point::operator-(const Vector &v){
	return Point(x-v.x, y-v.y, z-v.z);
}

Point &Point::operator-=(const Vector &v){
	x -= v.x; y -= v.y; z -= v.z;
	return *this;
}

Vector Point::operator-(const Point &p) const {
	return Vector(x-p.x, y-p.y, z-p.z);
}

//	multiplication
Point Point::operator*(float f) const{
	return Point(f*x, f*y, f*z);
}

Point &Point::operator*=(float f) {
	x *= f; y *= f; z *= f;
	return *this;
}

//	division
Point Point::operator/(float f) {
	assert(f != 0);
	float inv = 1.f / f;
	return Point(x * inv, y * inv, z * inv);
}

Point Point::operator/=(float f) {
	assert(f != 0);
	float inv = 1.f / f;
	x *= inv; y *= inv; z *= inv;
	return *this;
}

//	unary
Point Point::operator-() {
	return Point (-x, -y, -z);
}

float &Point::operator[](int index){
	if (index == 0)
		return x;
	else if (index == 1)
		return y;
	else
		return z;
}

const float Point::operator[](int index) const{
	if (index == 0)
		return x;
	else if (index == 1)
		return y;
	else
		return z;
}