//
//  shape.h
//  Aurora
//
//  Created by Espen Nordahl on 01/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_shape_h
#define Aurora_shape_h

#include "core.h"
#include "geometry.h"

#include "embree_accel.h"

#include <vector>

namespace Aurora {
	class Shape : public ReferenceCounted {
	public:
		Shape( const Transform *o2c, const Transform *c2o ) : objectToCamera(o2c), cameraToObject(c2o) { };
		
		virtual BBox objectBound() const = 0;
		virtual BBox worldBound() const = 0;
		
		virtual void dice( std::vector<Reference<Geometry> > &diced) = 0;
        
        virtual void makeEmbree(embree::BuildTriangle* triangles, embree::BuildVertex* vertices, std::vector<Vector> &normals, int *currentTri, int *currentVertex, int attributeIndex) = 0;

        
        virtual int numTriangles() = 0;
        virtual int numVertices() = 0;

	protected:
		const Transform *objectToCamera;
		const Transform *cameraToObject;
	};
}

#endif