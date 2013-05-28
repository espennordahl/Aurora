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
#include "attributeState.h"

#include <vector>

namespace Aurora {
	class Shape : public ReferenceCounted {
	public:
		Shape( const Transform *o2c, const Transform *c2o ) : objectToCamera(o2c), cameraToObject(c2o) { };
		
		virtual ~Shape(){};
            // Splits the geometry into triangles
		virtual void dice( std::vector<Reference<Geometry> > &diced) = 0;
        
            // Converts the shape into an embree object. 
            // TODO: This is pretty hacky, and can placed in the 
            // AuroraObject for a cleaner interface
        virtual void makeEmbree(embree::BuildTriangle* triangles, embree::BuildVertex* vertices, std::vector<Vector> &normals, std::vector< uv > &uvs, int *currentTri, int *currentVertex, AttributeState *attrs, int attributeIndex) = 0;

            // Handy information 
        virtual int numTriangles() = 0;
        virtual int numVertices() = 0;
        virtual BBox objectBound() const = 0;
		virtual BBox worldBound() const = 0;

            // Object transforms. It's important that if the object moves,
            // that we create new transforms as they're potentially shared
            // for effeciency
		const Transform *objectToCamera;
		const Transform *cameraToObject;
	};
}

#endif