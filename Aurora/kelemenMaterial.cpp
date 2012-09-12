//
//  kelemenMaterial.cpp
//  Aurora
//
//  Created by Espen Nordahl on 23/07/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>

#include "kelemenMaterial.h"
#include "cookTorrance.h"
#include "lambert.h"
#include "log.h"
#define lcontext LOG_KelemenMaterial

#include <math.h>

using namespace Aurora;

#define ALBEDO_SAMPLES 12

KelemenMaterial::KelemenMaterial( Color diffCol, Color specCol, float _exponent, float _reflectance, int numSamples){
	reflectance = _reflectance;
    exponent = _exponent;
	specBrdf = new CookTorrance(specCol, exponent, std::max(numSamples, 64));
	diffBrdf = new Lambert(diffCol, std::max(numSamples, 64));
    
    specGain = specCol.lum();
}

Reference<Brdf> KelemenMaterial::getBrdf( const Vector &Vn, const Vector &Nn ) const{
    if (reflectance == 1.f) return specBrdf;
        // compute fresnel
    float albedo = 0;
    for (int i = 0; i < ALBEDO_SAMPLES; i++) {
        float r1 = (float) rand()/RAND_MAX;
        float r2 = (float) rand()/RAND_MAX;
        
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
        float F = schlick(vdoth, reflectance);
        Vector Ln = (Hn * 2 * vdoth) - Vn;
        Ln = normalize(Ln);
        float a = F * std::max(0.f,dot(Ln, Nn))/vdoth;
        albedo += a/ALBEDO_SAMPLES;
    }
    if(albedo < 0.f && albedo > 1.f){
        LOG_ERROR("invalid albedo - outside of 0-1 range");
    }
        // importance sample brdfs
	float r = (float) rand()/RAND_MAX;
	if (r > albedo) {
		return diffBrdf;
	}
	else{
		return specBrdf;
	}
}