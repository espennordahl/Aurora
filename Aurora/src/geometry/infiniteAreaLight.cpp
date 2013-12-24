//
//  infiniteAreaLight.cpp
//  Aurora
//
//  Created by Espen Nordahl on 09/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>

#include "infiniteAreaLight.h"

#include <assert.h>
#include <math.h>

#define MAX_SAMPLE_RETRIES 64
#define SAMPLE_ROULETTE 0.9

using namespace Aurora;

InfiniteAreaLight::InfiniteAreaLight( Transform *o2c, Transform *c2o, Transform *o2w, Transform *w2o, Transform *c2w, Transform *w2c, float _exposure, Color color, std::string envmap, std::string name, RenderEnvironment *renderEnv) : Light(o2c, c2o, o2w, w2o, c2w, w2c, _exposure, color, name, renderEnv) {

    lightType = type_envLight;
    
    usingMap = false;
    if (envmap != "") {
        envMap = new Texture2D(envmap);
        envMap->initPdf();
        usingMap = true;
	}
    
}


Sample3D InfiniteAreaLight::generateSample( const Point &orig, const Vector &Nn, const IntegrationDomain &integrationDomain ){
    float x = (float) rand()/RAND_MAX;
    float y = (float) rand()/RAND_MAX;
	Vector dirT;
	Vector dirW;
	float pdf = 1.f;
    if (usingMap) {
        float vy = 1.f - 2.f * y;
        float r = sqrtf(std::max(0.f, 1.f - vy*vy));
        float phi = 2.f * M_PI * x;
        float vx = r * cosf(phi);
        float vz = r * sinf(phi);
        dirW = Vector(vx, vy, vz);
        dirW = (*worldToCamera)(dirW);
        pdf = envMap->pdf(x, y, 0.);
    } else {
        pdf = 2.f;
        dirT = SampleHemisphereUniform(x, y);
        dirW = tangentToWorld(dirT, Nn);
    }
    
	Sample3D s = Sample3D(Ray(normalize(dirW), orig, RAY_BIAS, 1000000.f), pdf / (M_PI * 4.), Color(0.));
	s.color = eval(s, Nn);
	return s;
}

Color InfiniteAreaLight::eval( const Sample3D &sample, const Vector &Nn ) {
	if (usingMap) {
        Vector dir = (*cameraToWorld)(sample.ray.direction);
        float v = 0.5 * (1.f - dir.y);//acosf(clamp(dir.y, -.99999, .99999)) * M_PI_INV;
        
        float u = atan2f(dir.z, dir.x) / (M_PI * 2.f);        
        if (u < 0.f) {
            u += 1.f;
        }
        
        Color c = envMap->read(u, v, 0.f);
        assert(c.hasNaNs() == false);
        return c * pow(2, m_exposure);
    }
    else {
        return color * pow(2, m_exposure);
    }
}

float InfiniteAreaLight::pdf( Sample3D *sample, const Vector &Nn, const IntegrationDomain &integrationDomain ) {
    if (usingMap) {
        Vector dir = (*cameraToWorld)(sample->ray.direction);
        float v = 0.5 * (1.f - dir.y);//acosf(clamp(dir.y, -.99999, .99999)) * M_PI_INV;
        
        float u = atan2f(dir.z, dir.x) / (M_PI * 2.f);
        if (u < 0.f) {
            u += 1.f;
        }

        float pdf = envMap->pdf(u, v, 1.f);
        if (integrationDomain == Hemisphere) {
            return pdf / (M_PI * 2.f);
        }
        else {            
            return pdf / (M_PI * 4.f);
        }
    }
    else {
        if (integrationDomain == Hemisphere) {
            return 1.f / (M_PI * 2.f);
        }
        else {
            return 1.f / (M_PI * 4.f);
        }
    }
}

bool InfiniteAreaLight::intersectBinary( Ray *ray ) const{
	return true;
}

void InfiniteAreaLight::makeRenderable(std::vector<RenderableTriangle> &renderable, AttributeState *attrs, int index){
	;
}