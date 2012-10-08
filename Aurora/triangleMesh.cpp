//
//  triangleMesh.cpp
//  Aurora
//
//  Created by Espen Nordahl on 07/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>

#include "triangleMesh.h"
#include "triangleGeo.h"

#include <assert.h>

using namespace Aurora;

TriangleMesh::TriangleMesh( const Transform *o2c, const Transform *c2o, 
						   int numTris, int numVerts, int numNorms, const int *vertIndex, const int *normIndex, const Point *Pin, const Vector *Nin, const uv *UVin) : 
							Shape(o2c, c2o){
								
	_numTriangles = numTris;
	_numVertices = numVerts;
	vertexIndex = new int[3 * _numTriangles];
	memcpy(vertexIndex, vertIndex, 3 * _numTriangles * sizeof(int));
    normalIndex = new int[3 * _numTriangles];
    memcpy(normalIndex, normIndex, 3 * _numTriangles * sizeof(int));                        
	P = new Point[ numVerts ];
    N = new Vector[ numNorms ];
    UV = new uv[ numTris* 3  ] ; // TODO: Add arbitrary primvar support        
	// transform vertices to world space, and calculate bbox
	// TODO: Default constructor for trBBox is weirding out..
	wBound = BBox((*objectToCamera)(Pin[0]));
	oBound = BBox(Pin[0]);
	for (int i = 0; i < _numVertices; i++) {
		P[i] = Pin[i];
		oBound.Union(P[i]);
		wBound.Union((*objectToCamera)(P[i]));
	}
    for (int i=0; i < numNorms; i++) { 
        N[i] = normalize(Nin[i]);
    }
    for (int i = 0; i < numTris * 3; i++) {                                    
        UV[i] = UVin[i];
    }
}

BBox TriangleMesh::objectBound() const{
	return oBound;
}

BBox TriangleMesh::worldBound() const{
	return wBound;
}

void TriangleMesh::dice( std::vector<Reference<Geometry> > &diced){
	for (int t=0; t < _numTriangles; t++) {
		Reference<Geometry> tri = new TriangleGeo(objectToCamera, cameraToObject, 
												  P[vertexIndex[t*3]], 
												  P[vertexIndex[t*3+1]], 
												  P[vertexIndex[t*3+2]],
                                                  N[normalIndex[t*3]],
                                                  N[normalIndex[t*3+1]],
                                                  N[normalIndex[t*3+2]],
                                                  UV[t*3],
                                                  UV[t*3+1],
                                                  UV[t*3+2]);
		diced.push_back(tri);
	}
}

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
        normals.push_back((*objectToCamera)(N[normalIndex[t*3]]));
        normals.push_back((*objectToCamera)(N[normalIndex[t*3+1]]));
        normals.push_back((*objectToCamera)(N[normalIndex[t*3+2]]));
        uvs.push_back(UV[t*3]);
        uvs.push_back(UV[t*3+1]);
        uvs.push_back(UV[t*3+2]);

    }
    *currentVertex += _numVertices;
    *currentTri += _numTriangles;
    attrs[attributeIndex].cameraToObject = cameraToObject;
    attrs[attributeIndex].objectToCamera = objectToCamera;
}