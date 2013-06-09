//
//  loopSubdivMesh.h
//  Aurora
//
//  Created by Espen Nordahl on 2/06/13.
//  Copyright (c) 2013 Espen Nordahl. All rights reserved.
//

#ifndef __Aurora__loopSubdivMesh__
#define __Aurora__loopSubdivMesh__

#include <iostream>
#include <set.h>

#include "shape.h"

#include "triangleMesh.h"


namespace Aurora{

    class PointCompare {
    public:
        bool operator()(const Point &a, const Point &b) {
            if (a.x != b.x){
                return a.x < b.x;
            }
            if (a.y != b.y){
                return a.y < b.y;
            }
            return a.z < b.z;
//            return a.x == b.x && a.y == b.y && a.z == b.z;
        }
    };

    
    struct SDVertex;
    struct SDFace;
    struct SDEdge;

    class LoopSubdivMesh : public Shape {
    public:
        LoopSubdivMesh(const Transform *o2c, const Transform *c2o, int nfaces, int nvertices, const int *vertexIndices, const Point *P, const uv *UVin, int nlevels);
        
        BBox objectBound() const{ return m_trimesh.objectBound(); }
		BBox worldBound() const{ return m_trimesh.worldBound(); }
        
        int numTriangles(){ return m_trimesh.numTriangles(); };
        int numVertices(){ return m_trimesh.numVertices(); };
        
        void makeEmbree(embree::BuildTriangle* triangles, embree::BuildVertex* vertices, std::vector<Vector> &normals, std::vector< uv > &uvs, int *currentTri, int *currentVertex, AttributeState *attrs, int attributeIndex){
            m_trimesh.makeEmbree(triangles, vertices, normals, uvs, currentTri, currentVertex, attrs, attributeIndex);
        };
        void dice( std::vector<std::tr1::shared_ptr<Geometry> > &diced){
            m_trimesh.dice(diced);
        }
        
    private:
        static float beta(int valence);
        static float gamma(int valence);

        void weightOneRing(set<Point, PointCompare> &unique, SDVertex *destVert,
                           SDVertex *vert, float beta) const;
        
        void weightBoundary(set<Point, PointCompare> &unique, SDVertex *destVert,
                            SDVertex *vert, float beta) const;
        
        TriangleMesh m_trimesh;
        
    };
}

#endif /* defined(__Aurora__loopSubdivMesh__) */
