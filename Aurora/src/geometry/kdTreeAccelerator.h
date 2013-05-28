//
//  kdTreeAccelerator.h
//  Aurora
//
//  Created by Espen Nordahl on 14/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_kdTreeAccelerator_h
#define Aurora_kdTreeAccelerator_h


#include "core.h"
#include "renderableTriangle.h"
#include "accelerationStructure.h"
#include "voxel.h"
#include <vector>

namespace Aurora {

	struct kdAccelNode{
		void initLeaf(uint32_t *primNums, int np){
			flags = 3;
			numPrims |= (np << 2);
			if (np == 0) {
				onePrimitive = 0;
			}
			else if (np == 1){
				onePrimitive = primNums[0];
			}
			else{
				primitives = new uint32_t[np];
				for (int i=0; i < np; i++) {
					primitives[i] = primNums[i];
				}
			}
		};
		
		void initInterior(uint32_t axis, uint32_t ac, float s){
			split = s;
			flags = axis;
			aboChild |= (ac << 2);	
		};
		
		float splitPos() const { return split; };
		uint32_t nPrimitives() const { return numPrims >> 2; };
		uint32_t splitAxis() const { return flags & 3; };
		bool isLeaf() const { return (flags & 3) == 3; };
		uint32_t aboveChild() const { return aboChild >> 2; };
		
		union {
			float split;
			uint32_t onePrimitive;
			uint32_t *primitives;
		};
	private:
		union{
			uint32_t flags;
			uint32_t numPrims;
			uint32_t aboChild;
		};
	};
	
	struct BoundEdge{
		BoundEdge(float tt, int pn, bool starting){
			t =tt;
			primNum = pn;
			type = starting ? START : END;
		}
		BoundEdge(){};
		bool operator<(const BoundEdge &e) const{
			if (t == e.t) {
				return (int)type < (int)e.type;
			}
			else return t < e.t;
		}
		
		float t;
		int primNum;
		enum{ START, END } type;
	};

	
	class KdTreeAccelerator: public AccelerationStructure {
	protected:
		int isectCost;
		int traversalCost;
		int maxPrims;
		int maxDepth;
		int nextFreeNode;
		int nAllocedNodes;
		float emptyBonus;
		BBox bounds;
		kdAccelNode *nodes;
		std::vector<RenderableTriangle> objects;
		
		void buildTree(int nodeNum, const BBox &nodeBounds, const std::vector<BBox> &allBounds, uint32_t *primNums, int nPrimitives, int depth, BoundEdge *edges[3], uint32_t *prims0, uint32_t *prims1, int badRefines);
		
	public:
		KdTreeAccelerator(){ };
		KdTreeAccelerator(const std::vector<RenderableTriangle> &objects, int icost, int tcost, float ebonus, int maxp, int md);
		
		bool intersect( Ray *ray, Intersection *intersection) const;
		bool intersectBinary( Ray *ray ) const;
        void getShadingTriangles( int triangleIndex, ShadingGeometry *shdGeo ){};
	};
}


#endif
