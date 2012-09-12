//
//  dummyAccelerationStructure.cpp
//  Aurora
//
//  Created by Espen Nordahl on 01/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>

#include "dummyAccelerationStructure.h"

using namespace Aurora;

DummyAccelerationStructure::DummyAccelerationStructure(const std::vector<RenderableTriangle> _geometry) : geometry(_geometry) {
	numGeo = (int)geometry.size();
}

bool DummyAccelerationStructure::intersect( Ray *ray, Intersection *intersection) const{
	bool hitSometing = false;
	for (int i=0; i < numGeo; i++) {
		if (geometry[i].intersect(ray, intersection)) {
			hitSometing = true;
		}
	}
	return hitSometing;
}

bool DummyAccelerationStructure::intersectBinary( Ray *ray ) const{
	for (int i=0; i < numGeo; i++) {
		if (geometry[i].intersectBinary(ray)) {
			return true;
		}
	}
	return false;	
}