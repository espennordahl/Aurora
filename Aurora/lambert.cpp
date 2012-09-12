//
//  lambert.cpp
//  Aurora
//
//  Created by Espen Nordahl on 06/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>

#include "lambert.h"

using namespace Aurora;

Lambert::Lambert(Color col, int _numSamples) : color(col), numSamples(_numSamples){
    brdfType = MatteBrdf;
    integrationDomain = Hemisphere;
    for (int t=0; t < NUM_THREADS; t++) {
        generateSampleBuffer(0,t);
        generateSampleBuffer(1,t);
        generateSampleBuffer(2,t);
    }
}

void Lambert::generateSampleBuffer(int i, int t){
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
    return Sample3D(Ray(normalize(tangentToWorld(dir, Nn)), Point(0), RAY_BIAS, 1000000.f), M_PI_INV * 0.5, color * M_PI_INV);
}

Color Lambert::evalSampleTangent(const Vector &Ln, const Vector &Vn){
	return color * M_PI_INV;
}

Color Lambert::evalSampleWorld(const Vector &Ln, const Vector &Vn, const Vector &Nn){
	float d = dot(Ln, Nn);
	if (d > 0.) {
		return color * M_PI_INV;
	}
	else{
		return Color(0);
	}
}

float Lambert::pdf(const Vector &Ln, const Vector &Vn, const Vector Nn) const{
	return M_PI_INV * 0.5;
}