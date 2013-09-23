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

#include "accel.h"
#include "intersector.h"

namespace Aurora {    
    
        // TODO: Clean up this messy class. 
    class EmbreeMesh {
    public:
        
        embree::BuildTriangle* triangles;
        embree::BuildVertex*   vertices;
        std::vector< Vector > normals;
        std::vector< uv > uvs;
        size_t numTriangles;
        size_t numVertices;
    
        std::vector<std::tr1::shared_ptr<Shape> > shapes;
        
        EmbreeMesh(){numTriangles = 0; numVertices = 0;};
        
        void appendTriangleMesh(std::tr1::shared_ptr<Shape> shape, int index){
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

            EmbreeStructure geometryStructure = {
                triangles,
                vertices,
                &normals,
                &uvs,
                &currentTri,
                &currentVertex
            };

            for (int i=0; i < shapes.size(); i++) {
                shapes[i]->makeEmbree(geometryStructure, attrs, i);
            }
        }
    };
}

#endif
