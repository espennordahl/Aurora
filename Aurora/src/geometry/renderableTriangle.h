//
//  renderableTriangle.h
//  Aurora
//
//  Created by Espen Nordahl on 07/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_renderableTriangle_h
#define Aurora_renderableTriangle_h

#include "core.h"
#include "renderableGeometry.h"
#include "triangleMesh.h"

namespace Aurora {
	class RenderableTriangle : public RenderableGeometry {
	public:
        RenderableTriangle(){};
		RenderableTriangle( const Point &p1, const Point &p2, const Point &p3, const Vector &n1, const Vector &n2, const Vector &n3, int _attributes );

		BBox worldBound() const;
		
		bool intersectBinary( Ray *ray ) const;
		bool intersect( Ray *ray, Intersection *intersection ) const;

		Point p1;
		Point p2;
		Point p3;
            // precomputed for intersection
		Vector e1;
		Vector e2;
            // normals
        Vector n1;
        Vector n2;
        Vector n3;
	};
}

#endif
