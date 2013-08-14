//
//  objTriangleMesh.cpp
//  Aurora
//
//  Created by Espen Nordahl on 07/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//


#include "objTriangleMesh.h"
#include "objParser.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <assert.h>

#include "log.h"
#define lcontext LOG_ObjMesh


using namespace Aurora;
using namespace std;


ObjTriangleMesh::ObjTriangleMesh( const Transform *o2c, const Transform *c2o, int subdlevels, const std::string &objfile ): Shape(o2c, c2o) {
	std::vector<Point> points;
    std::vector<Vector> normals;
    std::vector<uv> uvs;
	std::vector<int> vIndex;
    std::vector<int> nIndex;
    std::vector<int> uvIndex;

    ObjParser parser = ObjParser(objfile, &points, &normals, &uvs, &vIndex, &nIndex, &uvIndex);
    bool success = parser.parseFile();
	assert(success);
		
	size_t numTriangles = vIndex.size() / 3;
	size_t numVertices = vIndex.size();
    LOG_INFO("Found " << numTriangles << " triangles and " << numVertices << " vertices.");

    Point *P = new Point[ numVertices ];
    Vector *N = new Vector[ numVertices ];
    assert(nIndex.size() == vIndex.size());
    uv *UV = NULL;

    if(uvs.size()){
        assert(uvIndex.size() == vIndex.size());
        UV = new uv[ numVertices ];
    }

    
    int *vertexIndex = new int[3 * numTriangles];
	for (size_t i=0; i < 3*numTriangles; i++) {
		// obj files starts vert indices at 1 instead of 0.
		vertexIndex[i] = vIndex[i] - 1;
        nIndex[i]--;
        if(UV)
            uvIndex[i]--;

		assert(vertexIndex[i] < numVertices);
	}
	

	for (size_t i = 0; i < numVertices; i++) {
		P[i] = points[vertexIndex[i]];

        //N[i] = normals[nIndex[i]];
        N[i] = Vector(0,1,0);
        if (N[i].x == 0. && N[i].y == 0. && N[i].z == 0.) {
            LOG_WARNING("Found empty normal. Setting to 0,1,0");
            N[i] = Vector(0,1,0);
        }
        
        if (UV){
            UV[i] = uvs[uvIndex[i]];
        }
        
        vertexIndex[i] = (int)i;
	}

    if (subdlevels){
        m_shape = shared_ptr<Shape>(new LoopSubdivMesh(o2c, c2o, (int)numTriangles, (int)numVertices, vertexIndex, P, UV, subdlevels));
    } else {
        m_shape = shared_ptr<Shape>(new TriangleMesh(o2c, c2o, (int)numTriangles, (int)numVertices, vertexIndex, P, N, UV));
    }
    
    delete[] vertexIndex;
	delete[] P;
	delete[] N;
    if(UV)
        delete[] UV;
}

BBox ObjTriangleMesh::objectBound() const{
	return BBox();
}
BBox ObjTriangleMesh::worldBound() const{
	return BBox();
}
