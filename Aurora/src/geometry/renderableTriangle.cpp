//
//  renderableTriangle.cpp
//  Aurora
//
//  Created by Espen Nordahl on 07/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>

#include "renderableTriangle.h"

using namespace Aurora;

RenderableTriangle::RenderableTriangle( const Point &_p1, const Point &_p2, const Point &_p3, const Vector &v1, const Vector &v2, const Vector &v3, int _attributes ) : RenderableGeometry(_attributes){
	p1 = _p1;
	p2 = _p2;
	p3 = _p3;
	e1 = p2 - p1;
	e2 = p3 - p1;
    n1 = v1;
    n2 = v2;
    n3 = v3;
}


BBox RenderableTriangle::worldBound() const{
	BBox bounds = BBox(p1, p2);
	bounds.Union(p3);
	return bounds;
}

bool RenderableTriangle::intersectBinary( Ray *ray ) const{
	// compute s1
	Vector s1 = cross(ray->direction, e2);
	float divisor = dot(s1, e1);
	if (divisor == 0.) {
		return false;
	}
	
	float invDivisor = 1.f/divisor;
	
	// compute first barycentric coordinate
	
	Vector d = ray->origin - p1;
	float b1 = dot(d, s1) * invDivisor;
	if (b1 < 0. || b1 > 1.) {
		return false;
	}
	
	// compute second barycentric coordinate
	Vector s2 = cross(d, e1);
	float b2 = dot(ray->direction, s2) * invDivisor;
	if (b2 < 0. || (b1 + b2) > 1.) {
		return false;
	}
	
	// compute t to intersection point	
	float t = dot(e2,s2) * invDivisor;
	if (t < ray->mint || t > ray->maxt) {
		return false;
	}
	
	return true;

}

bool RenderableTriangle::intersect( Ray *ray, Intersection *intersection ) const{
	// compute s1
	Vector s1 = cross(ray->direction, e2);
	float divisor = dot(s1, e1);
	if (divisor == 0.) {
		return false;
	}
	
	float invDivisor = 1.f/divisor;
	
	// compute first barycentric coordinate
	
	Vector d = ray->origin - p1;
	float b1 = dot(d, s1) * invDivisor;
	if (b1 < 0. || b1 > 1.) {
		return false;
	}
	
	// compute second barycentric coordinate
	Vector s2 = cross(d, e1);
	float b2 = dot(ray->direction, s2) * invDivisor;
	if (b2 < 0. || (b1 + b2) > 1.) {
		return false;
	}
	
	// compute t to intersection point	
	float t = dot(e2,s2) * invDivisor;
	if (t < ray->mint || t > ray->maxt) {
		return false;
	}
	
	// Hit!
	ray->maxt = t;
	
	// fill in differential geo
	Point hitp = ray->origin + (ray->direction * t);
	
    
//	Vector norm = cross(e1, e2);
// compute final barycentric coordinate
    float b0 = 1 - b1 - b2;
    Vector norm = Vector(
                    b0 * n1[0] + b1 * n2[0] + b2 * n3[0],
                    b0 * n1[1] + b1 * n2[1] + b2 * n3[1],
                    b0 * n1[2] + b1 * n2[2] + b2 * n3[2]);
//	if (dot(norm, ray->direction) > 0.) {
//		norm *= -1;
//	}
	
	intersection->hitN = normalize(norm);
	intersection->hitP = hitp;
	intersection->attributesIndex = attributes;
    intersection->shdGeo.barCoords = Point(b0,b1,b2);
		
	return true;
}