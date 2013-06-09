//
//  triangleMesh.cpp
//  Aurora
//
//  Created by Espen Nordahl on 07/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include <set.h>

#include "triangleMesh.h"
#include "triangleGeo.h"

#include <assert.h>

using namespace Aurora;

TriangleMesh::TriangleMesh( const Transform *o2c, const Transform *c2o, 
						   int numTris, int numVerts, const int *vertIndex, const Point *Pin, const Vector *Nin, const uv *UVin) :
							Shape(o2c, c2o)
{
								
	_numTriangles = numTris;
	_numVertices = numVerts;
	vertexIndex = new int[3 * _numTriangles];
	memcpy(vertexIndex, vertIndex, 3 * _numTriangles * sizeof(int));

	P = new Point[ numVerts ];
    N = new Vector[ numVerts ];
    UV = new uv[ numVerts ];
	// transform vertices to world space, and calculate bbox
	// TODO: Default constructor for trBBox is weirding out..
	wBound = BBox((*objectToCamera)(Pin[0]));
	oBound = BBox(Pin[0]);
	for (int i = 0; i < _numVertices; i++) {
		P[i] = Pin[i];
		oBound.Union(P[i]);
		wBound.Union((*objectToCamera)(P[i]));
        N[i] = Nin[i];
        if(UVin){
            UV[i] = UVin[i];
        } else {
            UV[i] = uv();
        }
	}
}

BBox TriangleMesh::objectBound() const{
	return oBound;
}

BBox TriangleMesh::worldBound() const{
	return wBound;
}

struct DummyPoint{
    Point p;
    int index;
};

class DummyCompare {
public:
    bool operator()(const DummyPoint &a, const DummyPoint &b) {
        if (a.p.x != b.p.x){
            return a.p.x < b.p.x;
        }
        if (a.p.y != b.p.y){
            return a.p.y < b.p.y;
        }
        return a.p.z < b.p.z;
            //            return a.x == b.x && a.y == b.y && a.z == b.z;
    }
};


void TriangleMesh::makeEmbree(embree::BuildTriangle* triangles, embree::BuildVertex* vertices, std::vector<Vector> &normals, std::vector< uv > &uvs, int *currentTri, int *currentVertex, AttributeState *attrs, int attributeIndex){
    
    for (int v=0; v < _numVertices; v++) {
        Point p = (*objectToCamera)(P[v]);
        vertices[*currentVertex + v] = embree::BuildVertex(p.x, p.y, p.z);
    }
    
	for (int t=0; t < _numTriangles; t++) {

        triangles[*currentTri + t] = embree::BuildTriangle(*currentVertex + vertexIndex[t*3],
                                                           *currentVertex + vertexIndex[t*3+1],
                                                           *currentVertex + vertexIndex[t*3+2],
                                                           attributeIndex, *currentTri + t);
        normals.push_back((*objectToCamera)(N[vertexIndex[t*3]]));
        normals.push_back((*objectToCamera)(N[vertexIndex[t*3+1]]));
        normals.push_back((*objectToCamera)(N[vertexIndex[t*3+2]]));
        uvs.push_back(UV[vertexIndex[t*3]]);
        uvs.push_back(UV[vertexIndex[t*3+1]]);
        uvs.push_back(UV[vertexIndex[t*3+2]]);
    }
    *currentVertex += _numVertices;
    *currentTri += _numTriangles;
    attrs[attributeIndex].cameraToObject = cameraToObject;
    attrs[attributeIndex].objectToCamera = objectToCamera;
    
    delete[] P;
    delete[] UV;
    delete[] N;
    delete[] vertexIndex;
}