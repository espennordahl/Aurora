//
//  TriangleMesh.h
//  Aurora
//
//  Created by Espen Nordahl on 15/06/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_TriangleMesh_h
#define Aurora_TriangleMesh_h

#include "core.h"
#include "shape.h"

namespace Aurora {
	class TriangleMesh : public Shape{
    public:		
		BBox wBound;
		BBox oBound;
		int _numTriangles;
		int _numVertices;
		int *vertexIndex;
        int *normalIndex;
		Point *P;
        Vector *N;
		TriangleMesh( const Transform *o2c, const Transform *c2o, 
                     int numTris, int numVerts, int numNorms, const int *vertIndex, const int *normIndex, const Point *Pin, const Vector *Nin);
		
		BBox objectBound() const;
		BBox worldBound() const;
		
		void dice( std::vector<Reference<Geometry> > &diced) ;
        
        void makeEmbree(embree::BuildTriangle* triangles, embree::BuildVertex* vertices, std::vector<Vector> &normals, int *currentTri, int *currentVertex, int attributeIndex);

        int numTriangles(){ return _numTriangles; };
        int numVertices(){ return _numVertices; };
	};
}

#endif
