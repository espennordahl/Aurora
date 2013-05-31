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

Lambert::Lambert(std::string name, Color col, RenderEnvironment *renderEnv) :
Brdf(name, renderEnv),
color(col)
{
    brdfType = MatteBrdf;
    integrationDomain = Hemisphere;
}

#pragma mark -
#pragma mark Pipeline

void Lambert::frameBegin(){
    
}

void Lambert::frameEnd(){
    
}

Sample3D Lambert::getSample(const Vector &Vn, const Vector &Nn) const{
    float r1 = (float) rand()/RAND_MAX;
    float r2 = (float) rand()/RAND_MAX;
    
    Vector dir = SampleHemisphereUniform(r1, r2);
    return Sample3D(Ray(normalize(tangentToWorld(dir, Nn)), Point(0), RAY_BIAS, 1000000.f), M_PI_INV * 0.5, color * M_PI_INV);
}

Color Lambert::evalSampleTangent(const Vector &Ln, const Vector &Vn) const{
	return color * M_PI_INV;
}

Color Lambert::evalSampleWorld(const Vector &Ln, const Vector &Vn, const Vector &Nn) const{
	float d = dot(Ln, Nn);
	if (d > 0.) {
		return color * M_PI_INV ;
	}
	else{
		return Color(0);
	}
}

float Lambert::pdf(const Vector &Ln, const Vector &Vn, const Vector Nn) const{
	return M_PI_INV * 0.5;
}