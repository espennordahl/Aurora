//
//  uniformGridAccelerationStructure.h
//  Aurora
//
//  Created by Espen Nordahl on 07/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_uniformGridAccelerationStructure_h
#define Aurora_uniformGridAccelerationStructure_h

#include "core.h"
#include "renderableTriangle.h"
#include "accelerationStructure.h"
#include "voxel.h"
#include <vector>

namespace Aurora {
	class UniformGridAccelerator: public AccelerationStructure {
	protected:
		std::vector<RenderableTriangle> objects;
		BBox bounds;
		int nVoxels[3];
		Vector width;
		Vector invWidth;
		std::vector<Voxel> voxels;
		
		int posToVoxel(const Point &p, int axis) const;
		float voxelToPos(int p, int axis) const;
		
		int offset(int x, int y, int z) const;
        
	public:
		UniformGridAccelerator(){ };
		UniformGridAccelerator(const std::vector<RenderableTriangle> &objects);
        
        void getShadingTriangles( int triangleIndex, ShadingGeometry *shdGeo ){};
		bool intersect( Ray *ray, Intersection *intersection) const;
		bool intersectBinary( Ray *ray ) const;
	};
}


#endif
