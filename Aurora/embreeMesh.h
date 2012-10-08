//
//  embreeMesh.h
//  Aurora
//
//  Created by Espen Nordahl on 4/09/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_embreeMesh_h
#define Aurora_embreeMesh_h

#include "core.h"
#include "shape.h"

#include "embree_accel.h"
#include "embree_intersector.h"

namespace Aurora {    
    class EmbreeMesh {
    public:
        
        embree::BuildTriangle* triangles;
        embree::BuildVertex*   vertices;
        std::vector< Vector > normals;
        std::vector< uv > uvs;
        size_t numTriangles;
        size_t numVertices;
    
        std::vector<Reference<Shape> > shapes;
        
        void appendTriangleMesh(Reference<Shape> shape, int index){
            shapes.push_back(shape);
        }
    
        void preRender(AttributeState *attrs){
            /* allocate triangle buffers */
            numTriangles = 0;
            numVertices = 0;
            
            for (int i=0; i < shapes.size(); i++) {
                numTriangles += shapes[i]->numTriangles();
                numVertices += shapes[i]->numVertices();
            }
            
            triangles = (embree::BuildTriangle*) embree::rtcMalloc(numTriangles*sizeof(embree::BuildTriangle));
            vertices  = (embree::BuildVertex*  ) embree::rtcMalloc(numVertices *sizeof(embree::BuildVertex));

            int currentTri = 0;
            int currentVertex = 0;
            for (int i=0; i < shapes.size(); i++) {
                shapes[i]->makeEmbree(triangles, vertices, normals, uvs, &currentTri, &currentVertex, attrs, i);
            }
        }
    };
}

#endif
