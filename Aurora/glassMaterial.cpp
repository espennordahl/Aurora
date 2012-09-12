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

GlassMaterial::GlassMaterial( const Color &specCol, const Color &transCol, float _reflectance, float ior ):
reflectance(_reflectance) {
    refBrdf = new SpecMirror(specCol);
    transmitBrdf = new SpecTransmit(transCol, ior);
}

Reference<Brdf> GlassMaterial::getBrdf( const Vector &Vn, const Vector &Nn ) const{
    if (reflectance == 1.f) return refBrdf;
        // compute fresnel
    float costheta = dot(Vn,Nn) * 0.5;
    if (costheta < 0.f) {
        costheta = -costheta;
    }
    float albedo = schlick(costheta, reflectance);
        // importance sample brdfs
	float r = (float) rand()/RAND_MAX;
	if (r > albedo) {
		return transmitBrdf;
	}
	else{
		return refBrdf;
	}
}
