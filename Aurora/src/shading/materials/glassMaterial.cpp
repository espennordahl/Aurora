//
//  glassMaterial.cpp
//  Aurora
//
//  Created by Espen Nordahl on 3/09/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>

#include "glassMaterial.h"
#include "specTransmit.h"
#include "specMirror.h"

using namespace Aurora;

GlassMaterial::GlassMaterial( std::string name, const Color &specCol, const Color &transCol,
                             float _reflectance, float ior, int normalIndex, RenderEnvironment *renderEnv ):
Material(name, renderEnv, normalIndex),
reflectance(_reflectance) {
    refBrdf = new SpecMirror(name + ":specMirror", specCol, renderEnv);
    transmitBrdf = new SpecTransmit(name + ":specTransmit", transCol, ior, renderEnv);
}

void GlassMaterial::frameBegin(){
    
}

void GlassMaterial::frameEnd(){
    
}

BrdfState GlassMaterial::getBrdf( const Vector &Vn, const Vector &Nn, const ShadingGeometry &shdGeo, bool mattePath) {
    BrdfState state;
    state.parameters = new bxdfParameters;
    if (reflectance == 1.f){
        state.brdf = refBrdf;
        return state;
    }
        // compute fresnel
    float costheta = dot(Vn,Nn) * 0.5;
    if (costheta < 0.f) {
        costheta = -costheta;
    }
    float albedo = schlick(costheta, reflectance);
        // importance sample brdfs
	float r = (float) rand()/RAND_MAX;
	if (r > albedo) {
        state.brdf = transmitBrdf;
		return state;
	}
	else{
        state.brdf = refBrdf;
		return state;
	}
}
