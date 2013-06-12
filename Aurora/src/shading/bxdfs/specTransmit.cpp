//
//  specTransmit.cpp
//  Aurora
//
//  Created by Espen Nordahl on 3/09/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>

#include "specTransmit.h"

using namespace Aurora;

SpecTransmit::SpecTransmit(std::string name, Color _col, float _ior, RenderEnvironment *renderEnv):
Brdf(name, renderEnv),
color(_col), ior(_ior)
{
    m_brdfType = MirrorBrdf;
}

#pragma mark -
#pragma mark Pipeline

void SpecTransmit::frameBegin(){
    
}

void SpecTransmit::frameEnd(){
    
}



Sample3D SpecTransmit::getSample(const Vector &Vn, const Vector &Nn, bxdfParameters *parameters) const{
    float costheta = dot(Nn, Vn);
    float ei = 1;
    float et = ior;
    if (costheta < 0.) {
        ei = ior;
        et = 1.;
    }
    
    float sini2 = std::max(0.f, 1-costheta*costheta);
    float eta = ei / et;
    float sint2 = eta * eta * sini2;
    
    if (sint2 >= 1.) {
        return Sample3D(Ray(), 0., Color(0.));
    }
    
    float cost = sqrtf(std::max(0.f, 1-sint2));
    if (costheta > 0.) {
        cost = -cost;
    }
    float sintOverSini = eta;
    Vector Vl = worldToTangent(Vn, Nn);
    Vector T = Vector(sintOverSini * -Vl.x, sintOverSini * -Vl.y, cost);
    T = tangentToWorld(T, Nn);
    return Sample3D(Ray(normalize(T), Point(0.), RAY_BIAS, 10000000.f), 1., color);
}

Color SpecTransmit::evalSampleTangent(const Vector &Ln, const Vector &Vn, bxdfParameters *parameters) const{
    return Color(0.);
}

Color SpecTransmit::evalSampleWorld(const Vector &Ln, const Vector &Vn, const Vector &Nn, bxdfParameters *parameters) const{
    return Color(0.);
}

float SpecTransmit::pdf(const Vector &Ln, const Vector &Vn, const Vector Nn, bxdfParameters *parameters) const{
    return 0;
}
