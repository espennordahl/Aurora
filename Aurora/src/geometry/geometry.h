//
//  geometry.h
//  Aurora
//
//  Created by Espen Nordahl on 01/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_geometry_h
#define Aurora_geometry_h

#include "core.h"

namespace Aurora {
	class RenderableTriangle;

	class Geometry {
	public:
		Geometry( const Transform *o2c, const Transform *c2o ) : objectToCamera(o2c), cameraToObject(c2o) { };
		virtual ~Geometry(){};
		virtual void makeRenderable( std::vector<RenderableTriangle> &renderable, int attributeIndex) = 0;
		
		virtual BBox objectBound() const = 0;
		virtual BBox worldBound() const = 0;
		
	protected:
		const Transform *objectToCamera;
		const Transform *cameraToObject;
		
	};
}

#endif
