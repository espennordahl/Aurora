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
                     int numTris, int numVerts, const int *vertIndex, const Point *Pin, const Vector *Nin, const uv *UVin);
		
        TriangleMesh(): Shape(NULL, NULL){};
        
		BBox objectBound() const;
		BBox worldBound() const;
		        
        void makeEmbree(EmbreeStructure &geometryOut, AttributeState *attrs, int attributeIndex);

        int numTriangles(){ return _numTriangles; };
        int numVertices(){ return _numVertices; };
        
        void clearCache();

    private:
        BBox wBound;
		BBox oBound;
		int _numTriangles;
		int _numVertices;
		int *vertexIndex;
		Point *P;
        Vector *N;
        uv *UV;
	};
}

#endif
