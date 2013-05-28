//
//  Ray.cpp
//  Aurora
//

#include <iostream>
#include "math.h"

#include "math.h"
#include "ray.h"

using namespace Aurora;

#pragma mark -
#pragma mark Initialization

Ray::Ray(){
	Ray(Vector(0,1,0), Point(0,0,0), 0.f, INFINITY);
}

Ray::Ray(const Vector &dir, const Point &orig, float min, float max){
	direction = dir;
	origin = orig;
	mint = min;
	maxt = max;
}