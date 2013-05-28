//
//  Transform.cpp
//  Aurora
//

#include <iostream>

#include "transform.h"
#include "math_utils.h"

using namespace Aurora;

#pragma mark -
#pragma mark Contstructors

Transform::Transform(){
	m = Matrix4x4( 1,0,0,0,
					 0,1,0,0,
					 0,0,1,0,
					 0,0,0,1);
	mInv = m;
}

Transform::Transform(const Matrix4x4 &_m){
	m = _m;
	mInv = m.inverse();
}

Transform::Transform(const Matrix4x4 &_m, const Matrix4x4 &inv){
	m = _m; mInv = inv;
}

#pragma mark -
#pragma mark Handy Methods

Transform Transform::inverse(const Transform &t){
	return Transform(t.mInv, t.m);
}

#pragma mark -
#pragma mark Translation

// Translate
void Transform::translate(const Vector &delta){
	m *= Matrix4x4(1, 0, 0, delta.x,
				   0, 1, 0, delta.y,
				   0, 0, 1, delta.z,
				   0, 0, 0, 1);
	
//	mInv =  Matrix4x4(1, 0, 0, -delta.x,
//					  0, 1, 0, -delta.y,
//					  0, 0, 1, -delta.z,
//					  0, 0, 0, 1) * mInv;
    mInv = m.inverse();

}

// Scale
void Transform::scale(float x, float y, float z){
	m *= Matrix4x4(x, 0, 0, 0,
				   0, y, 0, 0,
				   0, 0, z, 0,
				   0, 0, 0, 1);
//	mInv = Matrix4x4(1.f/x, 0,     0,     0,
//					  0,     1.f/y, 0,     0,
//					  0,     0,     1.f/z, 0,
//					  0,     0,     0,     1) * mInv;
    mInv = m.inverse();

}

// Rotate
void Transform::rotateX(float angle){
	float sin_t = sinf(radians(angle));
	float cos_t = cosf(radians(angle));
	Matrix4x4 _m = Matrix4x4(   1,		0,		0,		0,
								0,	cos_t,	-sin_t,		0,
								0,	sin_t,	cos_t,		0,
								0,		0,		0,		1);
	m *= _m;
	mInv = m.inverse();
}

void Transform::rotateY(float angle){
	float sin_t = sinf(radians(angle));
	float cos_t = cosf(radians(angle));
	Matrix4x4 _m = Matrix4x4(cos_t,	    0,	sin_t,		0,
								0,		1,		0,		0,
								-sin_t,	0,	cos_t,		0,
								0,		0,		0,		1);
	m *= _m;
	mInv = m.inverse();
}

void Transform::rotateZ(float angle){
	float sin_t = sinf(radians(angle));
	float cos_t = cosf(radians(angle));
	Matrix4x4 _m = Matrix4x4(cos_t,-sin_t,	0,		0,
								sin_t, cos_t,	0,		0,
								0,		0,		1,		0,
								0,		0,		0,		1);
	m *= _m;
	mInv = m.inverse();
}

void Transform::rotate(float angle, const Vector &axis){
	const Vector a = normalize(axis);
	const float s = sinf(angle);
	const float c = cosf(angle);
	Matrix4x4 mat;
	
	mat.m[0][0] = a.x * a.x + (1.f - a.x * a.x) *c;
	mat.m[0][1] = a.x * a.y * (1.f - c) - a.z * s;
	mat.m[0][2] = a.x * a.z * (1.f - c) + a.y * s;
	mat.m[0][3] = 0;
	
	mat.m[1][0] = a.x * a.y * (1.f - c) + a.z *s;
	mat.m[1][1] = a.y * a.y + (1.f - a.y * a.y) *c;
	mat.m[1][2] = a.y * a.z * (1.f - c) - a.x * s;
	mat.m[1][3] = 0;
	
	mat.m[2][0] = a.x * a.z * (1.f - c) - a.y * s;
	mat.m[2][1] = a.y * a.z * (1.f - c) + a.x * s;
	mat.m[2][2] = a.z * a.z + (1.f - a.z * a.z) *c;
	mat.m[2][3] = 0;
	
	mat.m[3][0] = 0;
	mat.m[3][1] = 0;
	mat.m[3][2] = 0;
	mat.m[3][3] = 1;
	
	m *= mat;
        //	mInv = mat.transpose() * mInv;
    mInv = m.inverse();

}

void Transform::lookAt(const Point &pos, const Point &look, const Vector &up){

	Vector dir = normalize(look - pos);
	Vector left = normalize(cross(normalize(up), dir));
	Vector newUp = cross(dir, left);
	Matrix4x4 camToWorld;
	
	camToWorld.m[0][0] = left.x;
	camToWorld.m[1][0] = left.y;
	camToWorld.m[2][0] = left.z;
	camToWorld.m[3][0] = 0.;

	camToWorld.m[0][1] = newUp.x;
	camToWorld.m[1][1] = newUp.y;
	camToWorld.m[2][1] = newUp.z;
	camToWorld.m[3][1] = 0;
	
	camToWorld.m[0][2] = dir.x;
	camToWorld.m[1][2] = dir.y;
	camToWorld.m[2][2] = dir.z;
	camToWorld.m[3][2] = 1;

	
	camToWorld.m[0][3] = pos.x;
	camToWorld.m[1][3] = pos.y;
	camToWorld.m[2][3] = pos.z;
	camToWorld.m[3][3] = 1;
	
	m *= camToWorld.inverse();
        //	mInv = camToWorld * mInv;
    mInv = m.inverse();

}

bool Transform::swapsHandedness() const{
	float det = ((m.m[0][0] *
				  (m.m[1][1] * m.m[2][2] -
				   m.m[1][2] * m.m[2][1])) - 
				 (m.m[0][1] *
				  (m.m[1][0] * m.m[2][2] -
				   m.m[1][2] * m.m[2][0])) +
				 (m.m[0][2] *
				  (m.m[1][0] * m.m[2][1] -
				   m.m[1][1] * m.m[2][0])));
	return (det < 0.f);
}

#pragma mark -
#pragma mark operators

// Combining multiple Transforms
Transform Transform::operator*(const Transform &t2){
	Matrix4x4 m1 = m * t2.m;
    Matrix4x4 tmp = t2.mInv;
	Matrix4x4 m2 = tmp * mInv;
	return Transform(m1, m2);
}

// Combining multiple Transforms
Transform &Transform::operator*=(const Transform &t2){
	m = m * t2.m;
    Matrix4x4 tmp = t2.mInv;
	mInv = tmp * mInv;
	return *this;
}


// Points
Point Transform::operator()(const Point &p) const{
	float x = p.x; float y = p.y; float z = p.z;
	float xp = m.m[0][0]*x + m.m[0][1]*y + m.m[0][2]*z + m.m[0][3];
	float yp = m.m[1][0]*x + m.m[1][1]*y + m.m[1][2]*z + m.m[1][3];
	float zp = m.m[2][0]*x + m.m[2][1]*y + m.m[2][2]*z + m.m[2][3];
	float wp = m.m[3][0]*x + m.m[3][1]*y + m.m[3][2]*z + m.m[3][3];
	if (wp == 1.f) return Point(xp, yp, zp);
	return Point(xp, yp, zp)/wp;
}

void Transform::operator()(const Point &p, Point *ptrans) const{
	float x = p.x; float y = p.y; float z = p.z;
	ptrans->x = m.m[0][0]*x + m.m[0][1]*y + m.m[0][2]*z + m.m[0][3];
	ptrans->y = m.m[1][0]*x + m.m[1][1]*y + m.m[1][2]*z + m.m[1][3];
	ptrans->z = m.m[2][0]*x + m.m[2][1]*y + m.m[2][2]*z + m.m[2][3];
	float wp = m.m[3][0]*x + m.m[3][1]*y + m.m[3][2]*z + m.m[3][3];
	if (wp != 1.f) *ptrans /= wp;
}

// Vectors
Vector Transform::operator()(const Vector &v) const{
	float x = v.x; float y = v.y; float z = v.z;
	float xv = m.m[0][0]*x + m.m[0][1]*y + m.m[0][2]*z;
	float yv = m.m[1][0]*x + m.m[1][1]*y + m.m[1][2]*z;
	float zv = m.m[2][0]*x + m.m[2][1]*y + m.m[2][2]*z;
	return Vector(xv, yv, zv);
}

void Transform::operator()(const Vector &v, Vector *vtrans) const{
	float x = v.x; float y = v.y; float z = v.z;
	vtrans->x = m.m[0][0]*x + m.m[0][1]*y + m.m[0][2]*z;
	vtrans->y = m.m[1][0]*x + m.m[1][1]*y + m.m[1][2]*z;
	vtrans->z = m.m[2][0]*x + m.m[2][1]*y + m.m[2][2]*z;	
}

// Rays
Ray Transform::operator()(const Ray &r) const{
	Ray ret = r;
	(*this)(ret.origin, &ret.origin);
	(*this)(ret.direction, &ret.direction);
	return ret;
}

void Transform::operator()(const Ray &r, Ray *rtrans) const{
	Ray ret = r;
	(*this)(ret.origin, &ret.origin);
	(*this)(ret.direction, &ret.direction);

	rtrans->origin = ret.origin;
	rtrans->direction = ret.direction;
	rtrans->mint = ret.mint;
	rtrans->maxt = ret.maxt;
}



// BBox
BBox Transform::operator()(const BBox &b) const{
	const Transform &M = *this;
	BBox result(M(Point(b.pMin.x, b.pMin.y, b.pMin.z)));
	result.Union(Point(b.pMax.x, b.pMin.y, b.pMin.z));
	result.Union(Point(b.pMin.x, b.pMax.y, b.pMin.z));
	result.Union(Point(b.pMin.x, b.pMin.y, b.pMax.z));
	result.Union(Point(b.pMin.x, b.pMax.y, b.pMax.z));
	result.Union(Point(b.pMax.x, b.pMax.y, b.pMin.z));
	result.Union(Point(b.pMax.x, b.pMin.y, b.pMax.z));
	result.Union(Point(b.pMax.x, b.pMax.y, b.pMax.z));
	return result;
}

void Transform::operator()(const BBox &b, BBox *btrans) const{
	const Transform &M = *this;
	BBox result(M(Point(b.pMin.x, b.pMin.y, b.pMin.z)));
	result.Union(Point(b.pMax.x, b.pMin.y, b.pMin.z));
	result.Union(Point(b.pMin.x, b.pMax.y, b.pMin.z));
	result.Union(Point(b.pMin.x, b.pMin.y, b.pMax.z));
	result.Union(Point(b.pMin.x, b.pMax.y, b.pMax.z));
	result.Union(Point(b.pMax.x, b.pMax.y, b.pMin.z));
	result.Union(Point(b.pMax.x, b.pMin.y, b.pMax.z));
	result.Union(Point(b.pMax.x, b.pMax.y, b.pMax.z));
	btrans = &result;
}