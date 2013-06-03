//
//  BBox.cpp
//  Aurora
//

#include <iostream>
#include "math.h"

#include "math_utils.h"
#include "bbox.h"
#include "vector.h"

#include "log.h"
#define lcontext LOG_BBox

using namespace Aurora;

#pragma mark -
#pragma mark Initializers

BBox::BBox( const Point &p1, const Point &p2 ){
	pMin = Point(std::min(p1.x, p2.x), std::min(p1.y, p2.y), std::min(p1.z, p2.z)); 
	pMax = Point(std::max(p1.x, p2.x), std::max(p1.y, p2.y), std::max(p1.z, p2.z));
}

BBox::BBox( const Point &p ){
	pMin = p;
	pMax = p;
}

BBox::BBox(){
	pMin = Point(10000.f);
	pMax = Point(-10000.f);
}

#pragma mark -
#pragma mark Public Methods

// union()
// changes the BBox to ecompasses itself and a Point.
void BBox::Union( const Point &p ){
	pMin.x = std::min(pMin.x, p.x);
	pMin.y = std::min(pMin.y, p.y);
	pMin.z = std::min(pMin.z, p.z);
	pMax.x = std::max(pMax.x, p.x);
	pMax.y = std::max(pMax.y, p.y);
	pMax.z = std::max(pMax.z, p.z);
}


// union()
// combines BBox with a second BBoxes
void BBox::Union( const BBox &b2 ){
	pMin.x = std::min(pMin.x, b2.pMin.x);
	pMin.y = std::min(pMin.y, b2.pMin.y);
	pMin.z = std::min(pMin.z, b2.pMin.z);
	pMax.x = std::max(pMax.x, b2.pMax.x);
	pMax.y = std::max(pMax.y, b2.pMax.y);
	pMax.z = std::max(pMax.z, b2.pMax.z);
}

// overlaps()
// checks if two BBoxes overlaps.
bool BBox::overlaps(const BBox &b ) const {
	bool x = (pMax.x >= b.pMin.x) && (pMin.x <= b.pMax.x);
	bool y = (pMax.y >= b.pMin.y) && (pMin.y <= b.pMax.y);
	bool z = (pMax.z >= b.pMin.z) && (pMin.z <= b.pMax.z);
	return (x && y && z);
}

// inside()
// checks if a Point is inside the BBox.
bool BBox::inside( const Point &p) const{
	LOG_DEBUG("Testing if Point " << p << " is inside BBox.");
	return (p.x >= pMin.x && p.x <= pMax.x &&
			p.y >= pMin.y && p.y <= pMax.y &&
			p.z >= pMin.z && p.z <= pMax.z);
}

// expand()
// offsets each axis by a float input.
void BBox::expand( float delta ) {
	LOG_DEBUG("Expanding BBox by " << delta);
	pMin -= Vector(delta);
	pMax += Vector(delta);
}

// area()
// returns the surface area of the box.
float BBox::area() const{
	Vector d = pMax - pMin;
	return 2.f * (d.x*d.y + d.x*d.z + d.y*d.z);
}


float BBox::volume() const{
	Vector d = pMax - pMin;
	return d.x * d.y * d.z;
}

// maxExtent()
// Returns an integer representing the longest axis.
int BBox::maxExtent() const{
	Vector d = pMax - pMin;
	if (d.x > d.y && d.x > d.z)
		return 0;
	else if (d.y > d.z)
		return 1;
	else
		return 2;
}

Point BBox::lerp(float x, float y, float z) const{
	return Point(flerp(x, pMin.x, pMax.x), 
				   flerp(y, pMin.y, pMax.y),
				   flerp(z, pMin.z, pMax.z));
}

void BBox::boudingSphere( Point *c, float *rad ) const{
	*c = .5f * pMin;
    *c += .5f * pMax;
	*rad = inside(*c) ? distance(*c, pMax) : 0.0f;
}

float BBox::surfaceArea() const {
	Vector d = pMax - pMin;
	return 2.f * (d.x * d.y + d.x * d.z + d.y * d.z);
}

bool BBox::intersectP(const Ray &Ray, float *thit0, float *thit1) const{
	float t0 = Ray.mint, t1 = Ray.maxt;
	for (int i=0; i < 3; i++) {
		//update interval for ith bouding box slab
		float invRayDir = 1.f/Ray.direction[i];
		float tNear = (pMin[i] - Ray.origin[i]) * invRayDir;
		float tFar = (pMax[i] - Ray.origin[i]) * invRayDir;
		
		if (tNear > tFar) swap(tNear, tFar);
		// written as conditional to handle NaNs in invRayDir;
		t0 = tNear > t0 ? tNear : t0;
		t1 = tFar  < t1 ? tFar  : t1;
		if (t0 > t1)
			return false;
	}
	*thit0 = t0;
	*thit1 = t1;
	return true;
}