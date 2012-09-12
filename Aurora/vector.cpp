//
//  Vector.cpp
//  Aurora
//
//	Abstract:
//	Standard 3d Vector.


#include <iostream>
#include <assert.h>
#include <math.h>

#include "vector.h"

#include "log.h"
#define lcontext LOG_Vector

using namespace Aurora;

#pragma mark -
#pragma mark Constructors

/* constructors */
Vector::Vector(float _x, float _y, float _z):
x(_x), y(_y), z(_z) {
	// check for NaNs
//	assert(!hasNaNs());
}

Vector::Vector( float f ){
	Vector(f, f, f);
}

Vector::Vector(){
	Vector(0,1,0);
}

#pragma mark -
#pragma mark Handy Methods

bool Vector::hasNaNs(){
	return (isnan(x) || isnan(y) || isnan(z));
}


float Vector::lengthSquared() const{
	return x*x + y*y + z*z;
}

float Vector::length() const{
	return sqrtf(lengthSquared());
}

#pragma mark -
#pragma mark Symbol Overloading

//	addition 
Vector Vector::operator+(const Vector &v) const{
	return Vector(x+v.x, y+v.y, z+v.z);
}

Vector &Vector::operator+=(const Vector &v){
	x += v.x; y += v.y; z += v.z;
	return *this;
}

//	subtraction
Vector Vector::operator-(const Vector &v) const{
	return Vector(x-v.x, y-v.y, z-v.z);
}

Vector &Vector::operator-=(const Vector &v){
	x -= v.x; y -= v.y; z -= v.z;
	return *this;
}

//	multiplication
Vector Vector::operator*(float f) const{
	return Vector(f*x, f*y, f*z);
}

Vector &Vector::operator*=(float f) {
	x *= f; y *= f; z *= f;
	return *this;
}

//	division
Vector Vector::operator/(float f) const{
//	assert(f != 0);
	float inv = 1.f / f;
	return Vector(x * inv, y * inv, z * inv);
}

Vector Vector::operator/=(float f) {
//	assert(f != 0);
	float inv = 1.f / f;
	x *= inv; y *= inv; z *= inv;
	return *this;
}

//	unary
Vector Vector::operator-() {
	return Vector (-x, -y, -z);
}

float& Vector::operator[](int index){
	if (index == 0)
		return x;
	else if (index == 1)
		return y;
	else if (index == 2)
		return z;
	LOG_ERROR("Operator access out of range");
	return x;
}

const float& Vector::operator[](int index) const{
	if (index == 0)
		return x;
	else if (index == 1)
		return y;
	else if (index == 2)
		return z;
	LOG_ERROR("Operator access out of range");
	return x;
}