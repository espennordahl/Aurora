//
//  lambert.cpp
//  Aurora
//
//  Created by Espen Nordahl on 06/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>

#include "lambert.h"
#include "shader.h"
#include "log.h"
#define lcontext LOG_Lambert

using namespace Aurora;

Lambert::Lambert(std::string name, Color col, int _numSamples) : Brdf(name), numSamples(_numSamples){
    brdfType = MatteBrdf;
    integrationDomain = Hemisphere;
    weight = 1;
    for (int t=0; t < NUM_THREADS; t++) {
        generateSampleBuffer(0,t);
        generateSampleBuffer(1,t);
        generateSampleBuffer(2,t);
        color[t] = col;
    }
}

#pragma mark -
#pragma mark Pipeline

void Lambert::frameBegin(){
    
}

void Lambert::frameEnd(){
    
}


void Lambert::setParameters(brdfParameters *params, int thread){
    lambertParameters *newParams = (lambertParameters *) params;
    Color tmpCol = newParams->albedo;
    color[thread] = tmpCol;
}

void Lambert::generateSampleBuffer(int i, int t){
    for (int j=0; j < numSamples; j++) {
        float offsetX = (float) rand()/RAND_MAX;
        float offsetY = (float) rand()/RAND_MAX;
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

Sample3D Lambert::getSample(const Vector &Vn, const Vector &Nn, int depth, int thread){
    float r1, r2;
    if (depth < 3) {
        if (randomU[thread][depth].size() == 0) {
            generateSampleBuffer(depth, thread);
        }
        r1 = randomU[thread][depth].back();
        randomU[thread][depth].pop_back();
        r2 = randomV[thread][depth].back();
        randomV[thread][depth].pop_back();
	}
    else {
        r1 = (float) rand()/RAND_MAX;
        r2 = (float) rand()/RAND_MAX;
    }
    
    Vector dir = SampleHemisphereUniform(r1, r2);
    return Sample3D(Ray(normalize(tangentToWorld(dir, Nn)), Point(0), RAY_BIAS, 1000000.f), M_PI_INV * 0.5, color[thread] * weight * M_PI_INV);
}

Color Lambert::evalSampleTangent(const Vector &Ln, const Vector &Vn, int thread){
	return color[thread] * weight * M_PI_INV;
}

Color Lambert::evalSampleWorld(const Vector &Ln, const Vector &Vn, const Vector &Nn, int thread){
	float d = dot(Ln, Nn);
	if (d > 0.) {
		return color[thread] * weight * M_PI_INV ;
	}
	else{
		return Color(0);
	}
}

float Lambert::pdf(const Vector &Ln, const Vector &Vn, const Vector Nn, int thread) const{
	return M_PI_INV * 0.5;
}