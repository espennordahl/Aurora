//
//  cookTorrance.cpp
//  Aurora
//
//  Created by Espen Nordahl on 23/07/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>

#include "cookTorrance.h"
#include "math.h"

using namespace Aurora;

CookTorrance::CookTorrance(Color col, float _exponent, int _numSamples){
    brdfType = SpecBrdf;
    integrationDomain = Hemisphere;
	color = col;
	exponent = _exponent;
    numSamples = _numSamples;
    for (int t=0; t < NUM_THREADS; t++) {
        generateSampleBuffer(0,t);
        generateSampleBuffer(1,t);
        generateSampleBuffer(2,t);
    }
}

void CookTorrance::generateSampleBuffer(int i, int t){
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



Sample3D CookTorrance::getSample(const Vector &Vn, const Vector &Nn, int depth, int thread){
    
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
    
    float z = pow(r1, 1.f/(exponent+1.f));
    float r = sqrtf(fmax(0.f, 1.f - z*z));
//    float r = sinf(acosf(z));
    float phi = M_PI * 2 * r2;
    float x = r * cosf(phi);
    float y = r * sinf(phi);
    
    Vector dir = normalize(Vector(x,y,z));
    
    Vector Hn = normalize(tangentToWorld(dir, Nn));
    if (dot(Hn, Vn) < 0.) {
        Hn = -Hn;
    }
    
    float vdoth = dot(Vn, Hn);

    Vector Ln = normalize(Vector((Hn * 2 * vdoth) - Vn));
    
    float D = ((exponent + 1.f) / (M_PI * 2)) * pow(dot(Nn, Hn), exponent);
    float pdf = D / ( 4 * vdoth );
	return Sample3D(Ray(Ln, Point(0.f), RAY_BIAS, 10000000.f), pdf, evalSampleWorld(Ln, Vn, Nn));
}

float CookTorrance::pdf(const Vector &Ln, const Vector &Vn, const Vector Nn) const{

    Vector Hn = normalize(Ln + Vn);
    
    float vdoth = dot(Vn, Hn);
    
    if (vdoth <= 0.) {
        return 0;
    }
    
    float G = ((exponent + 1.) / (M_PI * 2)) * pow(dot(Nn, Hn), exponent);
    return G / (4 * vdoth );
}


Color CookTorrance::evalSampleTangent(const Vector &Ln, const Vector &Vn){
    return Color(1);
}

Color CookTorrance::evalSampleWorld(const Vector &Ln, const Vector &Vn, const Vector &Nn){
    
    Vector Hn = normalize(Ln + Vn);
    float vdotn = dot(Vn, Nn);
//    float ldotn = dot(Ln, Nn);
//    float vdotn = dot(Vn, Nn);
    float ndoth = dot(Hn, Nn);
    float ldotn = dot(Ln, Nn);
    float ldoth = dot(Ln, Hn);
    
    float D = ((exponent + 2.) / (M_PI * 2)) * pow(ndoth, exponent);
    
    if (vdotn <= 0. || ldotn <= 0.) {
        return Color(0.f);
    }
    return color * D / (4. * ldoth * ldoth);
}