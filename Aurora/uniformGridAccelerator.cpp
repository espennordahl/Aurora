//
//  uniformGridAccelerator.cpp
//  Aurora
//
//  Created by Espen Nordahl on 07/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>

#include "uniformGridAccelerator.h"

#include "log.h"
#define lcontext LOG_Gridaccel


using namespace Aurora;

UniformGridAccelerator::UniformGridAccelerator(const std::vector<RenderableTriangle> &_objects){
	// initialize primitives with primitives for grid
	objects = _objects;
	
	LOG_INFO("Creating Acceleration structure for " << objects.size() << " primitives.");
	// compute bounds and choose resolution
	bounds = objects[0].worldBound();
	for (uint32_t i=0; i < objects.size(); i++) {
		bounds.Union(objects[i].worldBound());
	}
	Vector delta = bounds.pMax - bounds.pMin;
	
	LOG_INFO("Bounds : " << bounds);
	
	// find voxelsPerUnit for grid
	int maxAxis = bounds.maxExtent();
	float invMaxWidth = 1.f/delta[maxAxis];
	float cubeRoot = 3.f * powf(float(objects.size()), 1.f/3.f);
	float voxelsPerUnitDist = cubeRoot * invMaxWidth;
	for (int axis = 0; axis < 3; axis++) {
		nVoxels[axis] = (int)floor(delta[axis] * voxelsPerUnitDist + 0.5);
		nVoxels[axis] = clamp(nVoxels[axis], 1, 64);
	}
	
	// compute voxel widths and allocate voxels
	for (int axis = 0; axis < 3; axis++) {
		width[axis] = delta[axis] / nVoxels[axis];
		invWidth[axis] = (width[axis] == 0.f) ? 0.f : 1.f / width[axis];
	}
	
	LOG_INFO("Creating " << nVoxels[0] * nVoxels[1] * nVoxels[2] << " voxels.");
	LOG_INFO("X: " << nVoxels[0] << ". Y: " << nVoxels[1] << ". Z: " << nVoxels[2] << ".");
	// TODO: better strategy for empty voxels
	// resize vectors
	voxels.resize(nVoxels[0]*nVoxels[1]*nVoxels[2]);
	
	// add empty voxels
	for (int x=0; x < nVoxels[0]; x++) {
		for (int y=0; y < nVoxels[1]; y++) {
			for (int z=0; z < nVoxels[2]; z++) {
				voxels[offset(x, y, z)] = Voxel();
			}
		}
	}
	
	// add primitives to voxels
	for (uint32_t i =0; i < objects.size(); i++) {
		// find voxel extent of primitive
		BBox primbound = objects[i].worldBound();
		int vmin[3], vmax[3];
		for (int axis = 0; axis < 3; axis++) {
			vmin[axis] = posToVoxel(primbound.pMin, axis);
			vmax[axis] = posToVoxel(primbound.pMax, axis);
		}
		
		// add primitive to overlapping voxels
		for (int z = vmin[2]; z <= vmax[2]; z++) {
			for (int y = vmin[1]; y <= vmax[1]; y++) {
				for (int x = vmin[0]; x <= vmax[0]; x++) {
					//					LOG_DEBUG("Adding primitive to voxel: " << x << ", " << y << ", " << z << ".");
					voxels[offset(x, y, z)].addObject(objects[i]);
				}
			}
		}
	}
	
	// TODO:(?) create reader-writer mutex for grid

}

bool UniformGridAccelerator::intersect( Ray *ray, Intersection *intersection) const{
	// If the ray starts inside, rayT is mint. If not,
	// we check if the ray hits the bbox. If it does,
	// we pick up rayT along the way, if not we exit.
	float rayT, tmp;
	if (bounds.inside(Point(ray->origin + ray->direction * ray->mint))) {
		rayT = ray->mint;
	}
	else if(!bounds.intersectP(*ray, &rayT, &tmp)){
		return false;
	}
	Point gridIntersect = ray->origin + ray->direction * rayT;
	
	// set up 3d DDA for ray 
	float nextCrossingT[3], deltaT[3];
	int step[3], out[3], pos[3];
	for (int axis=0; axis < 3; axis++) {
		// compute the current voxel for axis
		pos[axis] = posToVoxel(gridIntersect, axis);
		if (ray->direction[axis] >=0) {
			// handle ray with positive direction for voxel step
			nextCrossingT[axis] = rayT + 
			(voxelToPos(pos[axis]+1, axis) - gridIntersect[axis]) / ray->direction[axis];
			deltaT[axis] = width[axis] / ray->direction[axis];
			step[axis] = 1;
			out[axis] = nVoxels[axis];
		}
		else{
			// handle ray with negative direction for voxel step
			nextCrossingT[axis] = rayT + 
			(voxelToPos(pos[axis], axis) - gridIntersect[axis]) / ray->direction[axis];
			deltaT[axis] = -width[axis] / ray->direction[axis];
			step[axis] = -1;
			out[axis] = -1;
		}
	}
	
	// walk ray through voxel grid
	bool hitSomething = false;
	for (;;) {
		// check for intersection in current voxel and advance to next
		Voxel voxel = voxels[offset(pos[0], pos[1], pos[2])];
		if (voxel.numObjs()) {
			hitSomething |= voxel.intersect(ray, intersection);
		}
		// advance to next voxel
		// find stepAxis for stepping to next voxel
		int stepAxis = 2;
		if (nextCrossingT[2] > nextCrossingT[1] && nextCrossingT[0] > nextCrossingT[1]) {
			stepAxis = 1;
		}
		else if(nextCrossingT[0] < nextCrossingT[2]){
			stepAxis = 0;
		}
		if (ray->maxt < nextCrossingT[stepAxis]) {
			break;
		}
		pos[stepAxis] += step[stepAxis];
		if (pos[stepAxis] == out[stepAxis]) {
			break;
		}
		nextCrossingT[stepAxis] += deltaT[stepAxis];
	}
	
	return hitSomething;

}

bool UniformGridAccelerator::intersectBinary( Ray *ray ) const{
	// If the ray starts inside, rayT is mint. If not,
	// we check if the ray hits the bbox. If it does,
	// we pick up rayT along the way, if not we exit.
	float rayT, tmp;
	if (bounds.inside(Point(ray->origin + ray->direction * ray->mint))) {
		rayT = ray->mint;
	}
	else if(!bounds.intersectP(*ray, &rayT, &tmp)){
		return false;
	}
	Point gridIntersect = ray->origin + ray->direction * rayT;
	
	// set up 3d DDA for ray 
	float nextCrossingT[3], deltaT[3];
	int step[3], out[3], pos[3];
	for (int axis=0; axis < 3; axis++) {
		// compute the current voxel for axis
		pos[axis] = posToVoxel(gridIntersect, axis);
		if (ray->direction[axis] >=0) {
			// handle ray with positive direction for voxel step
			nextCrossingT[axis] = rayT + 
			(voxelToPos(pos[axis]+1, axis) - gridIntersect[axis]) / ray->direction[axis];
			deltaT[axis] = width[axis] / ray->direction[axis];
			step[axis] = 1;
			out[axis] = nVoxels[axis];
		}
		else{
			// handle ray with negative direction for voxel step
			nextCrossingT[axis] = rayT + 
			(voxelToPos(pos[axis], axis) - gridIntersect[axis]) / ray->direction[axis];
			deltaT[axis] = -width[axis] / ray->direction[axis];
			step[axis] = -1;
			out[axis] = -1;
		}
	}
	
	// walk ray through voxel grid
	for (;;) {
		// check for intersection in current voxel and advance to next
		Voxel voxel = voxels[offset(pos[0], pos[1], pos[2])];
		if (voxel.numObjs()) {
			if (voxel.intersectBinary(ray)){
				return true;
			}
		}
		// advance to next voxel
		// find stepAxis for stepping to next voxel
		int stepAxis = 2;
		if (nextCrossingT[2] > nextCrossingT[1] && nextCrossingT[0] > nextCrossingT[1]) {
			stepAxis = 1;
		}
		else if(nextCrossingT[2] > nextCrossingT[0]){
			stepAxis = 0;
		}
		
		if (ray->maxt < nextCrossingT[stepAxis]) {
			break;
		}
		pos[stepAxis] += step[stepAxis];
		if (pos[stepAxis] == out[stepAxis]) {
			break;
		}
		nextCrossingT[stepAxis] += deltaT[stepAxis];
	}
	
	return false;
}

#pragma mark -
#pragma mark Utility functions

int UniformGridAccelerator::posToVoxel(const Point &p, int axis) const{
	int v = (int)((p[axis] - bounds.pMin[axis]) * invWidth[axis]);
	return clamp(v, 0, nVoxels[axis]-1);
}

float UniformGridAccelerator::voxelToPos(int p, int axis) const{
	return bounds.pMin[axis] + p * width[axis];
}

int UniformGridAccelerator::offset(int x, int y, int z) const{
	return z*nVoxels[0]*nVoxels[1] + y*nVoxels[0] + x;
}