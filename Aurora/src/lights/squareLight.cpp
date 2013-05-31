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

SquareLight::SquareLight( Transform *o2c, Transform *c2o, Transform *o2w, Transform *w2o, Transform *c2w, Transform *w2c, float exposure, Color color, float sizeX, float sizeY, int _numSamples, std::string name, RenderEnvironment *renderEnv) : Light(o2c, c2o, o2w, w2o, c2w, w2c, exposure, color, _numSamples, name, renderEnv) {
	
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

    numSamples = std::max(numSamples, 64);
    for (int t=0; t < NUM_THREADS; t++) {
        generateSampleBuffer(0,t);
        generateSampleBuffer(1,t);
        generateSampleBuffer(2,t);
    }
}

void SquareLight::generateSampleBuffer(int i, int t){
    float offsetX = (float) rand()/RAND_MAX;
    float offsetY = (float) rand()/RAND_MAX;
    for (int j=0; j < numSamples; j++) {
        randomU[t][i].push_back((offsetX + (float)j)/numSamples);
        randomV[t][i].push_back((offsetY + (float)j)/numSamples);
    }
    for (int j=0; j < numSamples; j++) {
        int x = rand() % numSamples;
        float tmp = randomU[t][i][j];
        randomU[t][i][j] = randomU[t][i][x];
        randomU[t][i][x] = tmp;
        
        x = rand() % numSamples;
        tmp = randomV[t][i][j];
        randomV[t][i][j] = randomV[t][i][x];
        randomV[t][i][x] = tmp;
    }
}

Sample3D SquareLight::generateSample( const Point &orig, const Vector &Nn, const IntegrationDomain &integrationDomain, int depth, int thread){
    float x, y;
    if (depth < 3) {
        if (randomU[thread][depth].size() == 0) {
            generateSampleBuffer(depth, thread);
        }
        x = randomU[thread][depth].back();
        randomU[thread][depth].pop_back();
        y = randomV[thread][depth].back();
        randomV[thread][depth].pop_back();
	}
    else {
        x = (float) rand()/RAND_MAX;
        y = (float) rand()/RAND_MAX;
    }
	Point lightP = Point((x-0.5) * xScale * 2, (y-0.5) * yScale * 2, 0.f);
	lightP = (*objectToCamera)(lightP);
	Vector dir = lightP - orig;
    float sampleLength = dir.length();
    dir = normalize(dir);
	Sample3D s = Sample3D(Ray(dir, orig, RAY_BIAS, sampleLength - (RAY_BIAS*10)));
    float d = dot(lightN, dir);
    if (d < 0.) {
        d = -d;
    }
    else {
        d = 0.f;
    }
    s.color = eval(s, Nn);
    if (d == 0.) {
        s.pdf = 0.;
    }
    else{
        s.pdf = (sampleLength*sampleLength) / (d * xScale * yScale * 4.);
    }
	return s;
}

Color SquareLight::eval( const Sample3D &sample, const Vector &Nn ) {
//	Vector Ln = (*objectToCamera)(Vector(0,0,1));
//    float d = dot(normalize(sample.ray.direction), Ln);
//	if (d < 0.f)
//		d = 0;//-dot;
	Color col =  color * powf(2,exposure);
	return col;
}

float SquareLight::pdf( Sample3D *sample, const Vector &Nn, const IntegrationDomain &integrationDomain){
	if (intersectBinary(&sample->ray)) {
        float d = dot(lightN, normalize(sample->ray.direction));
        if (d < 0.) {
            d = -d;
        }
        float pdf = (sample->ray.maxt * sample->ray.maxt) / (d * xScale * yScale * 4.);
        sample->ray.maxt -= RAY_BIAS*10;
		return pdf;
	}
	return 0.f;
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
	
	Reference<Material> black = new MatteMaterial("Not In Use - Should be EDF", 0, 1, renderEnv);
	attrs[index].material = black;
	attrs[index].emmision = color * powf(2,exposure);
}

Color SquareLight::emission(){
    return color * powf(2,exposure);
}

Reference<Shape> SquareLight::shape(){
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
    
    Reference<Shape> s = new TriangleMesh(objectToCamera, cameraToObject, 2, 4, 4, Index, Nindex, P, N, UV);

    return s;
}