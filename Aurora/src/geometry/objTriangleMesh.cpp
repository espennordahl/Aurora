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

bool ObjTriangleMesh::parseObjLine(const std::string &line, std::vector<Point> &points, std::vector<Vector> &normals, std::vector<uv> &uvs, std::vector<int> &vertIndex, std::vector<int> &normIndex, std::vector<int> &uvIndex){
	
	if (line.empty()){
		return true;
	}
	
	stringstream ss(stringstream::in | stringstream::out);
	ss.str(line);
	std::string oper;
	ss >> oper;
	// comments
	if (oper[0] == *"#") {
		return true;
	}
	// vertices
	else if( oper.compare("v") == 0){
		float x, y, z;
		ss >> x >> y >> z;
		points.push_back(Point(x, y, z));
	}
    // normals
    else if( oper.compare("vn") == 0){
		float x, y, z;
		ss >> x >> y >> z;
		normals.push_back(Vector(x, y, z));
	}
    // UVs
    // normals
    else if( oper.compare("vt") == 0){
		float u, v;
		ss >> u >> v;
        uv myUVs;
        myUVs.u = u;
        myUVs.v = v;
		uvs.push_back(myUVs);
	}

	// edges
	else if( oper.compare("f") == 0){
		string edge1;
		string edge2;
		string edge3;
		ss >> edge1 >> edge2 >> edge3;
            // vert
		string buffer;
		for (int i=0; i < edge1.length(); i++) {
			if (edge1[i] == *"/") {
				break;
			}
			else {
				buffer = buffer + edge1[i];
			}
		}
		vertIndex.push_back(atoi(buffer.c_str()));
		buffer = "";
		for (int i=0; i < edge2.length(); i++) {
			if (edge2[i] == *"/") {
				break;
			}
			else {
				buffer = buffer + edge2[i];
			}
		}
		vertIndex.push_back(atoi(buffer.c_str()));
		
		buffer = "";
		for (int i=0; i < edge3.length(); i++) {
			if (edge3[i] == *"/") {
				break;
			}
			else {
				buffer = buffer + edge3[i];
			}
		}
		vertIndex.push_back(atoi(buffer.c_str()));
    
        // uv
        bool begin = false;
        buffer = "";
		for (int i=0; i < edge1.length(); i++) {
			if (begin == true) {
                if (edge1[i] == *"/") {
                    break;
                }
                buffer = buffer + edge1[i];
            }
            else if (edge1[i] == *"/") {
                begin = true;
            }
        }
        uvIndex.push_back(atoi(buffer.c_str()));
        begin = false;
        buffer = "";
        
        for (int i=0; i < edge2.length(); i++) {
			if (begin == true) {
                if (edge2[i] == *"/") {
                    break;
                }
                buffer = buffer + edge2[i];
            }
            else if (edge2[i] == *"/") {
                begin = true;
            }
        }
        uvIndex.push_back(atoi(buffer.c_str()));
        begin = false;
        buffer = "";
		for (int i=0; i < edge3.length(); i++) {
			if (begin == true) {
                if (edge3[i] == *"/") {
                    break;
                }
                buffer = buffer + edge3[i];
            }
            else if (edge3[i] == *"/") {
                begin = true;
            }
        }
        uvIndex.push_back(atoi(buffer.c_str()));

        
            // norm
        size_t found = edge1.find_last_of("/");
        edge1 = edge1.substr(found+1);
        normIndex.push_back(atoi(edge1.c_str()));
        
        found = edge2.find_last_of("/");
        edge2 = edge2.substr(found+1);
        normIndex.push_back(atoi(edge2.c_str()));
        
        found = edge3.find_last_of("/");
        edge3 = edge3.substr(found+1);
        normIndex.push_back(atoi(edge3.c_str()));

    }
    
	return true;
}

ObjTriangleMesh::ObjTriangleMesh( const Transform *o2c, const Transform *c2o, const std::string &objfile ): Shape(o2c, c2o) {
	std::vector<Point> points;
    std::vector<Vector> normals;
    std::vector<uv> uvs;
	std::vector<int> vertIndex;
    std::vector<int> nIndex;
    std::vector<int> uvIndex;
//    int i = 1;
//	char line[1024];
//	ifstream inFile(objfile.c_str(), ifstream::in);
//	if (!inFile) {
//	}
//	while (inFile.good()) {
//		inFile.getline(line, 1023);
//		if (!parseObjLine(string(line), points, normals, uvs, vertIndex, nIndex, uvIndex)) {
//		}
//		i++;
//	}
    ObjParser parser = ObjParser(objfile, &points, &normals, &uvs, &vertIndex, &nIndex, &uvIndex);
    bool success = parser.parseFile();
	assert(success);
	
//	uint32_t tmp = vertIndex.size();
	
	uint32_t numTriangles = (int)vertIndex.size() / 3;
	uint32_t numVertices = (int)points.size();
    uint32_t numNorms = (int)normals.size();
	int *vertexIndex = new int[3 * numTriangles];
    int *normalIndex = new int[3 * numTriangles];
//	vertexIndex[4] = 2;
//	vertexIndex[0] = 341;
//	assert(vertIndex.size() == (3*numTriangles));
//  assert(normals.size() == numVertices*3);
	for (uint32_t i=0; i < 3*numTriangles; i++) {
		// obj files starts vert indices at 1 instead of 0.
		vertexIndex[i] = vertIndex[i] - 1;
        normalIndex[i] = nIndex[i] - 1;
		assert(vertexIndex[i] < numVertices);
		//		LOG_WARNING("->" << vertexIndex[i]);
	}
	
	Point *P = new Point[ numVertices ];
    uv *UV = new uv[ numTriangles * 3 ];
    Vector *N = new Vector[ numNorms ];
	for (uint32_t i = 0; i < numVertices; i++) {
		P[i] = points[i];
//        LOG_DEBUG(P[i]);
	}
	for (uint32_t i = 0; i < numNorms; i++) {
        if (normals[i].x == 0. && normals[i].y == 0. && normals[i].z == 0.) {
            LOG_WARNING("Found empty normal. Setting to 0,1,0");
            normals[i] = Vector(0,1,0);
        }
        N[i] = normals[i];
    }
    bool hasUVs = true;
    if (uvs.size() == 0) {
        hasUVs = false;
        LOG_WARNING("Mesh has no UVs in file. Adding garbage UVs");
    }
    for (uint32_t i = 0; i < numTriangles*3; i++) {
        if (hasUVs) {
            UV[i] = uvs[uvIndex[i]-1];
        }
        else{
            uv myUV;
            myUV.u = 0;
            myUV.v = 0;
            UV[i] = myUV;
        }
    }
    
	m_shape = shared_ptr<Shape>(new TriangleMesh(o2c, c2o, numTriangles, numVertices, numNorms, vertexIndex, normalIndex, P, N, UV));
	free(P);
	free(N);
    free(UV);
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