//
//  renderableGeometry.h
//  Aurora
//
//  Created by Espen Nordahl on 01/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_renderableGeometry_h
#define Aurora_renderableGeometry_h

#include "core.h"
#include "geometry.h"
#include "intersection.h"

namespace Aurora {
	class RenderableGeometry {
	public:
		int attributes;
        RenderableGeometry(){};
		RenderableGeometry( int _attributes ):attributes(_attributes){ };
		
		virtual BBox worldBound() const = 0;
		
		virtual bool intersectBinary( Ray *ray ) const = 0;
		virtual bool intersect( Ray *ray, Intersection *intersection ) const = 0;
		
	};
}

#endif