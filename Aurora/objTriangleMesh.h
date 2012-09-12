//
//  objTriangleMesh.h
//  Aurora
//
//  Created by Espen Nordahl on 07/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_objTriangleMesh_h
#define Aurora_objTriangleMesh_h

#include "core.h"
#include "shape.h"
#include "triangleMesh.h"

namespace Aurora {
	class ObjTriangleMesh : public Shape{
	protected:
		Reference<Shape> shape;
		bool parseObjLine(const std::string &line, std::vector<Point> &points, std::vector<Vector> &normals, std::vector<int> &vertIndex, std::vector<int> &normIndex);
	public:		
		ObjTriangleMesh( const Transform *o2c, const Transform *c2o, const std::string &objfile );

		BBox objectBound() const;
		BBox worldBound() const;
		
		void dice( std::vector<Reference<Geometry> > &diced) ;
        
        void makeEmbree(embree::BuildTriangle* triangles, embree::BuildVertex* vertices, std::vector<Vector> &normals, int *currentTri, int *currentVertex, int attributeIndex){
            shape->makeEmbree(triangles, vertices, normals, currentTri, currentVertex, attributeIndex);
        };
        
        int numTriangles(){ return shape->numTriangles(); };
        int numVertices(){ return shape->numVertices(); };
	};
}

#endif