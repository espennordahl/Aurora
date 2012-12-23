//
//  blinn.cpp
//  Aurora
//
//  Created by Espen Nordahl on 12/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>

#include "blinn.h"

using namespace Aurora;

Blinn::Blinn(Color col, float _roughness, int _numSamples){
	color = col;
    integrationDomain = Hemisphere;
    brdfType = SpecBrdf;
	roughness = _roughness;
    numSamples = _numSamples;
    weight = 1;
    for (int t=0; t < NUM_THREADS; t++) {
        generateSampleBuffer(0,t);
        generateSampleBuffer(1,t);
        generateSampleBuffer(2,t);
    }
}

void Blinn::generateSampleBuffer(int i, int t){
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
    }
}

Sample3D Blinn::getSample(const Vector &Vn, const Vector &Nn, int depth, int thread){
	// Compute sampled half-angle vector $\wh$ for Blinn distribution
    float u1, u2;
    if (depth < 3) {
        if (randomU[thread][depth].size() == 0) {
            generateSampleBuffer(depth, thread);
        }
        u1 = randomU[thread][depth].back();
        randomU[thread][depth].pop_back();
        u2 = randomU[thread][depth].back();
        randomV[thread][depth].pop_back();
	}
    else {
        u1 = (float) rand()/RAND_MAX;
        u2 = (float) rand()/RAND_MAX;
    }
	float costheta = powf(u1, 1.f / (roughness+1));
	float sintheta = sqrtf(fmax(0.f, 1.f - costheta*costheta));
	float phi = u2 * 2.f * M_PI;
	Vector wh = tangentToWorld(SphericalDirection(sintheta, costheta, phi), Nn);
	if (dot(Vn, wh) < 0.f) wh = -wh;
	 
	// Compute incident direction by reflecting about $\wh$
	Vector R = (Vn*-1.f) + (wh * 2.f * dot(Vn, wh));
	 
	// Compute PDF for $\wi$ from Blinn distribution
	float blinn_pdf = ((roughness + 1.f) * powf(costheta, roughness)) /
	                       (2.f * M_PI * 4.f * dot(Vn, wh));
	if (dot(Vn, wh) <= 0.f) blinn_pdf = 0.f;

	return Sample3D(Ray(normalize(R), Point(0.), RAY_BIAS, 100000.f), blinn_pdf, evalSampleWorld(normalize(R), Vn, Nn, thread));
}

float Blinn::pdf(const Vector &Ln, const Vector &Vn, const Vector Nn, int thread) const{
	Vector wh = normalize(Vn + Ln);
	float costheta = dot(wh,Nn);
	// Compute PDF for $\wi$ from Blinn distribution
	float blinn_pdf = ((roughness + 1.f) * powf(costheta, roughness)) /
	                       (2.f * M_PI * 4.f * dot(Vn, wh));
	if (dot(Vn, wh) <= 0.f) blinn_pdf = 0.f;
		return blinn_pdf;
}


Color Blinn::evalSampleTangent(const Vector &Ln, const Vector &Vn, int thread){
	float cosThetaO = Vn.z;
	float cosThetaI = Ln.z;
	if (cosThetaI == 0.f || cosThetaO == 0.f) return Color(0.f);
	Vector wh = Ln + Vn;
	if (wh.x == 0. && wh.y == 0. && wh.z == 0.) return Color(0.f);
	wh = normalize(wh);
	float cosThetaH = dot(Ln, wh);	
	
	float NdotWh = wh.z;
	float NdotWo = Vn.z;
	float NdotWi = Ln.z;
	float WOdotWh = absDot(Vn, wh);
	float G = fmin(1.f, fmin((2.f * NdotWh * NdotWo / WOdotWh), (2.f * NdotWh * NdotWi / WOdotWh)));
	
	float D = (roughness+2) * INV_TWOPI * powf(cosThetaH, roughness);
	
	return color * D * G /
	(4.f * cosThetaI * cosThetaO);
}

Color Blinn::evalSampleWorld(const Vector &Ln, const Vector &Vn, const Vector &Nn, int thread){
	float cosThetaO = dot(Vn,Nn);
	float cosThetaI = dot(Ln,Nn);
	if (cosThetaI == 0.f || cosThetaO == 0.f) return Color(0.f);
	Vector wh = Ln + Vn;
	if (wh.x == 0. && wh.y == 0. && wh.z == 0.) return Color(0.f);
	wh = normalize(wh);
//	float cosThetaH = dot(Ln, wh);	
	
	float NdotWh = dot(wh,Nn);
	float NdotWo = dot(Vn,Nn);
	float NdotWi = dot(Ln,Nn);
	float WOdotWh = absDot(Vn, wh);
	
	float G = fmin(1.f, fmin((2.f * NdotWh * NdotWo / WOdotWh), (2.f * NdotWh * NdotWi / WOdotWh)));
	
	float D = (roughness+2) * INV_TWOPI * powf(NdotWh, roughness);
	
	return color * D * G /
	(4.f * cosThetaI * cosThetaO);

}