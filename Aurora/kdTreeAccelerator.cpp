//
//  kdTreeAccelerator.cpp
//  Aurora
//
//  Created by Espen Nordahl on 14/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include <algorithm>

#include "kdTreeAccelerator.h"

#include "log.h"
#define lcontext LOG_Gridaccel


using namespace Aurora;

KdTreeAccelerator::KdTreeAccelerator(const std::vector<RenderableTriangle> &_objects, int icost, int tcost, float ebonus, int maxp, int md): isectCost(icost), traversalCost(tcost), emptyBonus(ebonus), maxPrims(maxp), maxDepth(md), objects(_objects){
	
	//	build kd-tree
	int numObjects = (int)objects.size();
	LOG_INFO("Creating Acceleration structure for " << numObjects << " primitives.");

	nextFreeNode = nAllocedNodes = 0;
	if (maxDepth <= 0) {
		maxDepth = floor(8 + 1.3f * log2(float(numObjects)));
	}
	
	// compute bounds for construction
	bounds = BBox(objects[0].worldBound());
	std::vector<BBox> primBounds;
	primBounds.reserve(numObjects);
	for (uint32_t i=0; i < numObjects; i++) {
		BBox b = objects[i].worldBound();
		bounds.Union(b);
		primBounds.push_back(b);
	}
	
	// allocate working memory
	BoundEdge *edges[3];
	for (int i=0; i < 3; i++) {
		edges[i] = new BoundEdge[2*numObjects];
	}
	uint32_t *prims0 = new uint32_t[numObjects];
	uint32_t *prims1 = new uint32_t[(maxDepth + 1) * numObjects];
	
	// init primNums
	uint32_t *primNums = new uint32_t[numObjects];
	for (uint32_t i=0; i < numObjects; i++) {
		primNums[i] = i;
	}
	
	// start recursive construction
	buildTree(0, bounds, primBounds, primNums, numObjects, maxDepth, edges, prims0, prims1, 0);
	
	// free working memory
}

void KdTreeAccelerator::buildTree(int nodeNum, const BBox &nodeBounds, const std::vector<BBox> &allBounds, uint32_t *primNums, int nPrimitives, int depth, BoundEdge *edges[3], uint32_t *prims0, uint32_t *prims1, int badRefines){
	
	// get next free node from nodes array
	if (nextFreeNode == nAllocedNodes) {
		int nAlloc = fmax(2 * nAllocedNodes, 512);
		kdAccelNode *n = new kdAccelNode[nAlloc];
		if (nAllocedNodes > 0) {
			memcpy(n, nodes, nAllocedNodes * sizeof(kdAccelNode));
			free(nodes);
		}
		nodes = n;
		nAllocedNodes = nAlloc;
	}
	++nextFreeNode;
	
	// initialize leaf node if termination criteria met
	if (nPrimitives <= maxPrims || depth == 0) {
		nodes[nodeNum].initLeaf(primNums, nPrimitives);
		return;
	}
		
	//		initialize interior and continue recursion
	// choose split axis position for interior node
	int bestAxis = -1; 
	int bestOffset = -1;
	float bestCost = INFINITY;
	float oldCost = isectCost * (float)nPrimitives;
	float totalSA = nodeBounds.surfaceArea();
	float invTotalSA = 1.f/totalSA;
	Vector d = nodeBounds.pMax - nodeBounds.pMin;
	
	// choose which axis to split along
	uint32_t axis = nodeBounds.maxExtent();
	int retries = 0;
retrySplit:

	// init edges for axis
	for (int i = 0; i < nPrimitives; i++) {
		int pn = primNums[i];
		const BBox &bbox = allBounds[pn];
		edges[axis][2*i] = BoundEdge(bbox.pMin[axis], pn, true);
		edges[axis][2*i+1] = BoundEdge(bbox.pMax[axis], pn, false);
	}
	
	
	std::sort(&edges[axis][0], &edges[axis][2*nPrimitives]);
		
	
	// compute cost of all splits for axis to find the best
	int nBelow = 0;
	int nAbove = nPrimitives;
    int skipPrims = 1;
    if (nPrimitives > 1000) {
        skipPrims = 5;
        if (nPrimitives > 10000) {
            skipPrims = 10;
            if (nPrimitives > 1000000) {
                skipPrims = 100;
            }
        }
    }
	for (int i=0; i < 2*nPrimitives; i+=skipPrims) {
		if (edges[axis][i].type == BoundEdge::END) --nAbove;
		float edget = edges[axis][i].t;
		if (edget > nodeBounds.pMin[axis] &&
			edget < nodeBounds.pMax[axis]) {
			// compute cost for splitting at ith edge
			uint32_t otherAxis0 = (axis + 1) % 3, otherAxis1 = (axis + 2) % 3;
			float belowSA = 2 * (d[otherAxis0] * d[otherAxis1] +
								 (edget - nodeBounds.pMin[axis]) * 
								 (d[otherAxis0] + d[otherAxis1]));
			float aboveSA = 2 * (d[otherAxis0] * d[otherAxis1] +
								 (nodeBounds.pMax[axis] - edget) * 
								 (d[otherAxis0] + d[otherAxis1]));
			float pBelow = belowSA * invTotalSA;
			float pAbove = aboveSA * invTotalSA;
			float eb = (nAbove == 0 || pAbove == 0) ? emptyBonus : 0.f;
			float cost = traversalCost + isectCost * (1.f - eb) * (pBelow * pBelow + pAbove * pAbove);
			// update split if this is lower
			if (cost < bestCost) {
				bestCost = cost;
				bestAxis = axis;
				bestOffset = i;
			}
		}
		if (edges[axis][i].type == BoundEdge::START) ++nBelow;
	}
	
	// create leaf if no good splits were found
	if (bestAxis == -1 && retries < 2) {
		++retries;
		axis = (axis+1) % 3;
		goto retrySplit;
	}
	if (bestCost > oldCost) ++badRefines;
	if ((bestCost > 4.f * oldCost && nPrimitives < 16) ||
		bestAxis == -1 || badRefines == 3) {
		nodes[nodeNum].initLeaf(primNums, nPrimitives);
		return;
	}
	// classify primities with respect to split
	int n0 = 0;
	int n1 = 0;
	for (int i=0; i < bestOffset; i++) {
		if (edges[bestAxis][i].type == BoundEdge::START) {
			int tmp = edges[bestAxis][i].primNum;
			prims0[n0++] = tmp;
		}
	}
	for (int i=bestOffset+1; i < 2*nPrimitives; i++) {
		if (edges[bestAxis][i].type == BoundEdge::END) {
			prims1[n1++] = edges[bestAxis][i].primNum;
		}
	}
	
	// recursively init children nodes
	float tsplit = edges[bestAxis][bestOffset].t;
	BBox bounds0 = nodeBounds;
	BBox bounds1 = nodeBounds;
	bounds0.pMax[bestAxis] = bounds1.pMin[bestAxis] = tsplit;
	buildTree(nodeNum+1, bounds0, allBounds, prims0, n0, depth-1, edges, 
			  prims0, prims1 + nPrimitives, badRefines);
	uint32_t aboveChild = nextFreeNode;
	nodes[nodeNum].initInterior(bestAxis, aboveChild, tsplit);
	buildTree(aboveChild, bounds1, allBounds, prims1, n1, 
			  depth-1, edges, prims0, prims1 + nPrimitives, badRefines);
}

struct kdToDo{
	const kdAccelNode *node;
	float tmin, tmax;
};

bool KdTreeAccelerator::intersectBinary( Ray *ray ) const{

	// compute parametric range of ray inside kdtree extent
	float tmin, tmax;
	if (!bounds.intersectP(*ray, &tmin, &tmax)) {
		return false;
	}
	
	// prepare for traversal
	Vector invDir = Vector(1.f/ray->direction.x, 1.f/ray->direction.y, 1.f/ray->direction.z);
	kdToDo todo[maxDepth];
	int todoPos = 0;
	
	// traverse tree in order for ray
	const kdAccelNode *node = &nodes[0];
	while (node != NULL) {
		if (ray->maxt < tmin) {
			// bail out if we found a hit closer than node
			break;
		}
		if (!node->isLeaf()) {
			// process interoir node
			// compute parametric distance along ray to split plane
			int axis = node->splitAxis();
			float tplane = (node->splitPos() - ray->origin[axis]) * invDir[axis];
			
			// get node children pointers for ray
			const kdAccelNode *firstChild, *secondChild;
			int belowFirst = (ray->origin[axis] < node->splitPos()) || 
							 (ray->origin[axis] == node->splitPos() && ray->direction[axis] >= 0);
			if (belowFirst){
				firstChild = node + 1;
				secondChild = &nodes[node->aboveChild()];
			}
			else{
				firstChild = &nodes[node->aboveChild()];
				secondChild = node + 1;
			}
			
			// advance to next node, possibly enqueue other child
			if (tplane > tmax || tplane <= 0) {
				node = firstChild;
			}
			else if(tplane < tmin){
				node = secondChild;
			}
			else{
				// enqueue second child
				todo[todoPos].node = secondChild;
				todo[todoPos].tmin = tplane;
				todo[todoPos].tmax = tmax;
				++todoPos;
				
				node = firstChild;
				tmax = tplane;
			}
		}
		else{
			// check for intersections inside leaf node
			uint32_t nPrimitives = node->nPrimitives();
			if (nPrimitives == 1){
				const RenderableTriangle prim = objects[node->onePrimitive];
				if (prim.intersectBinary(ray)) {
					return true;
				}
			}
			else{
				uint32_t *prims = node->primitives;
				for (uint32_t i=0; i < nPrimitives; i++) {
					const RenderableTriangle prim = objects[prims[i]];
					if (prim.intersectBinary(ray)) {
						return true;
					}
				}
			}
			// grab next node from todo list
			if (todoPos > 0) {
				--todoPos;
				node = todo[todoPos].node;
				tmin = todo[todoPos].tmin;
				tmax = todo[todoPos].tmax;
			}
			else{
				break;
			}
		}
	}
	
	return false;
}


bool KdTreeAccelerator::intersect( Ray *ray, Intersection *intersection) const{
	
	// compute parametric range of ray inside kdtree extent
	float tmin, tmax;
	if (!bounds.intersectP(*ray, &tmin, &tmax)) {
		return false;
	}
	
	// prepare for traversal
	Vector invDir = Vector(1.f/ray->direction.x, 1.f/ray->direction.y, 1.f/ray->direction.z);
	kdToDo todo[maxDepth];
	int todoPos = 0;
	
	// traverse tree in order for ray
	bool hit = false;
	const kdAccelNode *node = &nodes[0];
	while (node != NULL) {
		if (ray->maxt < tmin) {
			// bail out if we found a hit closer than node
			break;
		}
		if (!node->isLeaf()) {
			// process interoir node
			// compute parametric distance along ray to split plane
			int axis = node->splitAxis();
			float tplane = (node->splitPos() - ray->origin[axis]) * invDir[axis];
			
			// get node children pointers for ray
			const kdAccelNode *firstChild, *secondChild;
			int belowFirst = (ray->origin[axis] < node->splitPos()) || 
			(ray->origin[axis] == node->splitPos() && ray->direction[axis] >= 0);

			if (belowFirst){
				firstChild = node + 1;
				secondChild = &nodes[node->aboveChild()];
			}
			else{
				firstChild = &nodes[node->aboveChild()];
				secondChild = node + 1;
			}
			
			// advance to next node, possibly enqueue other child
			if (tplane > tmax || tplane <= 0) {
				node = firstChild;
			}
			else if(tplane < tmin){
				node = secondChild;
			}
			else{
				// enqueue second child
				todo[todoPos].node = secondChild;
				todo[todoPos].tmin = tplane;
				todo[todoPos].tmax = tmax;
				++todoPos;
				
				node = firstChild;
				tmax = tplane;
			}
		}
		else{
			// check for intersections inside leaf node
			uint32_t nPrimitives = node->nPrimitives();
			if (nPrimitives == 1){
				const RenderableTriangle prim = objects[node->onePrimitive];
				if (prim.intersect(ray, intersection)) {
					hit = true;
				}
			}
			else{
				uint32_t *prims = node->primitives;
				for (uint32_t i=0; i < nPrimitives; i++) {
					uint32_t tmp = prims[i];
					const RenderableTriangle prim = objects[tmp];
					if (prim.intersect(ray, intersection)) {
						hit = true;
					}
				}
			}
			// grab next node from todo list
			if (todoPos > 0) {
				--todoPos;
				node = todo[todoPos].node;
				tmin = todo[todoPos].tmin;
				tmax = todo[todoPos].tmax;
			}
			else{
				break;
			}
		}
	}
	
	return hit;
}
