//
//  TriangleGeo.h
//  Aurora
//
//  Created by Espen Nordahl on 07/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_TriangleGeo_h
#define Aurora_TriangleGeo_h


#include "core.h"
#include "geometry.h"

namespace Aurora {
	class TriangleGeo : public Geometry {
	public:
		TriangleGeo( const Transform *o2c, const Transform *c2o, const Point &p1, const Point &p2, const Point &p3, const Vector &n1, const Vector &n2, const Vector &n3, const uv &uv1, const uv &uv2, const uv &uv3);
		
		void makeRenderable( std::vector<RenderableTriangle> &renderable, int attributeIndex);

		BBox objectBound() const;
		BBox worldBound() const;
		
	protected:
		Point p1;
		Point p2;
		Point p3;
        Vector n1;
        Vector n2;
        Vector n3;
        uv uv1;
        uv uv2;
        uv uv3;
	};
}



#endif