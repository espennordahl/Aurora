//
//  triangleGeo.cpp
//  Aurora
//
//  Created by Espen Nordahl on 07/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>

#include "TriangleGeo.h"
#include "renderableTriangle.h"

using namespace Aurora;

TriangleGeo::TriangleGeo( const Transform *o2c, const Transform *c2o, const Point &_p1, const Point &_p2, const Point &_p3, const Vector &v1, const Vector &v2, const Vector &v3, const uv &_uv1, const uv &_uv2, const uv &_uv3) : Geometry(o2c, c2o) {
	p1 = _p1; p2 = _p2; p3 = _p3;
    n1 = v1; n2 = v2; n3 = v3;
    uv1 = _uv1; uv2 = _uv2; uv3 = _uv3;
}

void TriangleGeo::makeRenderable( std::vector<RenderableTriangle> &renderable, int attributeIndex){
	RenderableTriangle tri = RenderableTriangle((*objectToCamera)(p1), (*objectToCamera)(p2), (*objectToCamera)(p3),
                                                (*objectToCamera)(n1), (*objectToCamera)(n2), (*objectToCamera)(n3), attributeIndex);
	renderable.push_back(tri);
}

BBox TriangleGeo::objectBound() const{
	BBox bounds = BBox(p1, p2);
	bounds.Union(p3);
	return bounds;

}

BBox TriangleGeo::worldBound() const{
	BBox bounds = BBox(p1, p2);
	bounds.Union(p3);
	(*objectToCamera)(bounds);
	return bounds;
}