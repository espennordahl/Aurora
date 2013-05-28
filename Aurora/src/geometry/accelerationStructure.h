//
//  accelerationStructure.h
//  Aurora
//
//  Created by Espen Nordahl on 01/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_accelerationStructure_h
#define Aurora_accelerationStructure_h

#include "core.h"
#include "renderableGeometry.h"
#include "renderableTriangle.h"

namespace Aurora {
	class AccelerationStructure{
	public:
		AccelerationStructure(){};
        virtual ~AccelerationStructure(){};
		AccelerationStructure(const std::vector<RenderableTriangle> geometry);
		
		virtual bool intersect( Ray *ray, Intersection *intersection) const = 0;
		virtual bool intersectBinary( Ray *ray ) const = 0;
        virtual void getShadingTriangles( int triangleIndex, ShadingGeometry *shdGeo ) = 0;

	};
}

#endif
