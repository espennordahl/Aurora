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
	std::vector<int> parse_vertIndex;
    std::vector<int> parse_nIndex;
    std::vector<int> parse_uvIndex;

    ObjParser parser = ObjParser(objfile, &points, &normals, &uvs, &parse_vertIndex, &parse_nIndex, &parse_uvIndex);
    bool success = parser.parseFile();
	assert(success);
		
	size_t numTriangles = parse_vertIndex.size() / 3;
	size_t numVertices = points.size();
    size_t numNorms = normals.size();
    size_t numUVs = uvs.size();
    LOG_INFO("Found " << numTriangles << " triangles and " << numVertices << " vertices.");
	int *vertexIndex = new int[3 * numTriangles];
    int *normalIndex = new int[3 * numTriangles];
    int *uvIndex = new int[3 * numTriangles];
	for (size_t i=0; i < 3*numTriangles; i++) {
		// obj files starts vert indices at 1 instead of 0.
		vertexIndex[i] = parse_vertIndex[i] - 1;
        normalIndex[i] = parse_nIndex[i] - 1;
        uvIndex[i] = parse_uvIndex[i]-1;
		assert(vertexIndex[i] < numVertices);
	}
	
	Point *P = new Point[ numVertices ];
    Vector *N = new Vector[ numNorms ];
    uv *UV = new uv[numUVs];

	for (size_t i = 0; i < numVertices; i++) {
		P[i] = points[i];
	}

	for (size_t i = 0; i < numNorms; i++) {
        if (normals[i].x == 0. && normals[i].y == 0. && normals[i].z == 0.) {
            LOG_WARNING("Found empty normal. Setting to 0,1,0");
            normals[i] = Vector(0,1,0);
        }
        N[i] = normals[i];
    }
    
    for(size_t i=0; i < numUVs; ++i){
        UV[i] = uvs[i];
    }
    
    if (subdlevels){
        Point *Ptmp = new Point[3 * numTriangles];
        uv *UVtmp = new uv[3*numTriangles];
        for (int i=0; i< 3 * numTriangles; ++i) {
            Ptmp[i] = P[vertexIndex[i]];
            UVtmp[i] = UV[uvIndex[i]];
            vertexIndex[i] = i;
        }
        numVertices = 3 * numTriangles;
        m_shape = shared_ptr<Shape>(new LoopSubdivMesh(o2c, c2o, (int)numTriangles, (int)numVertices, vertexIndex, Ptmp, UVtmp, subdlevels));
    } else {
        m_shape = shared_ptr<Shape>(new TriangleMesh(o2c, c2o, (int)numTriangles, (int)numVertices, (int)numNorms, (int)numUVs, vertexIndex, normalIndex, uvIndex, P, N, UV));
    }
    free(UV);
	free(P);
	free(N);
}

BBox ObjTriangleMesh::objectBound() const{
	return BBox();
}
BBox ObjTriangleMesh::worldBound() const{
	return BBox();
}

void ObjTriangleMesh::dice( std::vector<std::tr1::shared_ptr<Geometry> > &diced){
	m_shape->dice(diced);
}