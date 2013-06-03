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
		TriangleMesh( const Transform *o2c, const Transform *c2o,
                     int numTris, int numVerts, int numNorms, const int *vertIndex, const int *normIndex, const Point *Pin, const Vector *Nin, const uv *UVin);
		
        TriangleMesh(): Shape(NULL, NULL){};
        
		BBox objectBound() const;
		BBox worldBound() const;
		
		void dice( std::vector<std::tr1::shared_ptr<Geometry> > &diced) ;
        
        void makeEmbree(embree::BuildTriangle* triangles, embree::BuildVertex* vertices, std::vector<Vector> &normals, std::vector< uv > &uvs, int *currentTri, int *currentVertex, AttributeState *attrs, int attributeIndex);

        int numTriangles(){ return _numTriangles; };
        int numVertices(){ return _numVertices; };
        
        BBox wBound;
		BBox oBound;
		int _numTriangles;
		int _numVertices;
		int *vertexIndex;
        int *normalIndex;
		Point *P;
        Vector *N;
        uv *UV;
	};
}

#endif
