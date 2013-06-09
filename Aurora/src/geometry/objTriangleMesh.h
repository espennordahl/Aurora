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
#include "loopSubdivMesh.h"

namespace Aurora {
	class ObjTriangleMesh : public Shape{
    public:
		ObjTriangleMesh( const Transform *o2c, const Transform *c2o, int subdlevels, const std::string &objfile );
        
		BBox objectBound() const;
		BBox worldBound() const;
		        
        void makeEmbree(embree::BuildTriangle* triangles, embree::BuildVertex* vertices, std::vector<Vector> &normals, std::vector< uv > &uvs, int *currentTri, int *currentVertex, AttributeState *attrs, int attributeIndex){
            m_shape->makeEmbree(triangles, vertices, normals, uvs, currentTri, currentVertex, attrs, attributeIndex);
        };
        
        int numTriangles(){ return m_shape->numTriangles(); };
        int numVertices(){ return m_shape->numVertices(); };

	protected:        
        std::tr1::shared_ptr<Shape> m_shape;
	};
}

#endif