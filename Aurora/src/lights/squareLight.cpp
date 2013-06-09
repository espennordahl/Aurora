//
//  squareLight.cpp
//  Aurora
//
//  Created by Espen Nordahl on 05/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>

#include "attributeState.h"
#include "squareLight.h"
#include "matteMaterial.h"
#include "renderableTriangle.h"

using namespace Aurora;

tbb::atomic<int> SquareLight::m_halton_index = tbb::atomic<int>();

SquareLight::SquareLight( Transform *o2c, Transform *c2o, Transform *o2w, Transform *w2o, Transform *c2w, Transform *w2c, float exposure, Color color, float sizeX, float sizeY, std::string name, RenderEnvironment *renderEnv) : Light(o2c, c2o, o2w, w2o, c2w, w2c, exposure, color, name, renderEnv) {
	
    lightType = type_areaLight;
    
	xScale = sizeX;
	yScale = sizeY;
    
    Point p1 = Point(-xScale, -yScale, 0);
	Point p2 = Point(-xScale,yScale, 0);
	Point p3 = Point(xScale, yScale, 0);
	p1 = (*objectToCamera)(p1);
	p2 = (*objectToCamera)(p2);
	p3 = (*objectToCamera)(p3);
    
    Vector v1 = p1 - p2;
    Vector v2 = p2 - p3;
    
    lightN = normalize(cross(v1, v2));
    
    m_sampler.init_faure();
}


bool SquareLight::intersectBinary( Ray *ray ) const{
    
	Point p1 = Point(-xScale, -yScale, 0);
	Point p2 = Point(-xScale,yScale, 0);
	Point p3 = Point(xScale, yScale, 0);
	Point p4 = Point(xScale, -yScale, 0);
	p1 = (*objectToCamera)(p1);
	p2 = (*objectToCamera)(p2);
	p3 = (*objectToCamera)(p3);
	p4 = (*objectToCamera)(p4);
    Vector v = Vector(0,1,0);
	RenderableTriangle tri1 = RenderableTriangle(p1, p2, p4, v, v, v, 1);
	RenderableTriangle tri2 = RenderableTriangle(p3, p2, p4, v, v, v, 2);
    Intersection isect;
    if (tri1.intersect(ray, &isect) == true) {
        return true;
    }
    if (tri2.intersect(ray, &isect) == true) {
        return true;
    }
	return false;
}

void SquareLight::makeRenderable(std::vector<RenderableTriangle> &renderable, AttributeState *attrs, int index){
	Point p1 = Point(-xScale, -yScale, 0);
	Point p2 = Point(-xScale,yScale, 0);
	Point p3 = Point(xScale, yScale, 0);
	Point p4 = Point(xScale, -yScale, 0);
	p1 = (*objectToCamera)(p1);
	p2 = (*objectToCamera)(p2);
	p3 = (*objectToCamera)(p3);
	p4 = (*objectToCamera)(p4);
    Vector v = Vector(0,1,0);
	RenderableTriangle tri1 = RenderableTriangle(p1, p2, p4, v, v, v, index);
	RenderableTriangle tri2 = RenderableTriangle(p3, p2, p4, v, v, v, index);
	renderable.push_back(tri1);
	renderable.push_back(tri2);
	
	Material * black = new MatteMaterial("Not In Use - Should be EDF", 0, 1, m_renderEnv);
	attrs[index].material = black;
	attrs[index].emmision = color * powf(2,exposure);
}

Color SquareLight::emission(){
    return color * powf(2,exposure);
}

std::tr1::shared_ptr<Shape> SquareLight::shape(){
	Point p1 = Point(-xScale, -yScale, 0);
	Point p2 = Point(-xScale,yScale, 0);
	Point p3 = Point(xScale, yScale, 0);
	Point p4 = Point(xScale, -yScale, 0);
//	p1 = (*objectToCamera)(p1);
//	p2 = (*objectToCamera)(p2);
//	p3 = (*objectToCamera)(p3);
//	p4 = (*objectToCamera)(p4);

	Point P[4] = {p1, p2, p3, p4};
	
	int Index[6] = {0, 1, 3,
		2, 1, 3};   
    
    Vector N[2] = {lightN, lightN};
    uv tmpUV;
    uv UV[2] = {tmpUV, tmpUV};
    int Nindex[6] = {0, 0, 0, 0, 0, 0};
    int UVindex[6] = {0, 0, 0, 0, 0, 0};
    
    shared_ptr<Shape> s = shared_ptr<Shape>(new TriangleMesh(objectToCamera, cameraToObject, 2, 4, 4, 4, Index, Nindex, UVindex, P, N, UV));

    return s;
}