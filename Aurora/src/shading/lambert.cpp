//
//  lambert.cpp
//  Aurora
//
//  Created by Espen Nordahl on 06/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>

#include "material.h" // TODO: just here to get lambert struct..
#include "lambert.h"
#include "shader.h"
#include "log.h"
#define lcontext LOG_Lambert

using namespace Aurora;

tbb::atomic<int> Lambert::m_halton_index = tbb::atomic<int>();

Lambert::Lambert(std::string name, RenderEnvironment *renderEnv) :
Brdf(name, renderEnv)
{
    brdfType = MatteBrdf;
    integrationDomain = Hemisphere;
    m_sampler.init_faure();
}

#pragma mark -
#pragma mark Pipeline

void Lambert::frameBegin(){
    
}

void Lambert::frameEnd(){
    
}

Sample3D Lambert::getSample(const Vector &Vn, const Vector &Nn, const LambertParameters &parameters) const{
#ifdef USE_HALTON
    const int i = m_halton_index.fetch_and_increment();
    float r1 = m_sampler.sample(SURFACE_DOMAIN_X, i);
    float r2 = m_sampler.sample(SURFACE_DOMAIN_Y, i);
#else
    float r1 = (float) rand()/RAND_MAX;
    float r2 = (float) rand()/RAND_MAX;
#endif
    assert(r1 >= 0. && r1 <= 1.f);
    assert(r2 >= 0. && r2 <= 1.f);
    
    Vector dir = SampleHemisphereUniform(r1, r2);
    return Sample3D(Ray(normalize(tangentToWorld(dir, Nn)), Point(0), RAY_BIAS, 1000000.f), M_PI_INV * 0.5, parameters.albedo * M_PI_INV);
}

Color Lambert::evalSampleTangent(const Vector &Ln, const Vector &Vn, const LambertParameters &parameters) const{
	return parameters.albedo * M_PI_INV;
}

Color Lambert::evalSampleWorld(const Vector &Ln, const Vector &Vn, const Vector &Nn, const LambertParameters &parameters) const{
	float d = dot(Ln, Nn);
	if (d > 0.) {
		return parameters.albedo * M_PI_INV ;
	}
	else{
		return Color(0);
	}
}

float Lambert::pdf(const Vector &Ln, const Vector &Vn, const Vector Nn) const{
	return M_PI_INV * 0.5;
}