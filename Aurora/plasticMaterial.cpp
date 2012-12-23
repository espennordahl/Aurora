//
//  plasticMaterial.cpp
//  Aurora
//
//  Created by Espen Nordahl on 14/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>

#include "plasticMaterial.h"
#include "blinn.h"
#include "lambert.h"

using namespace Aurora;

PlasticMaterial::PlasticMaterial( const Color &col, float roughness, float _reflectance, float _topLayerThickness, int numSamples){
	reflectance = _reflectance;
	specGain = _topLayerThickness;
	specBrdf = new Blinn(Color(1), roughness, numSamples);
	diffBrdf = new Lambert(col, numSamples);
}

Reference<Brdf> PlasticMaterial::getBrdf( const Vector &Vn, const Vector &Nn, int thread ) {
	// compute fresnel
	Vector R = (Vn*-1.f) + (Nn * 2.f * dot(Vn, Nn));
	float VdotH = dot(Vn, normalize(R));
	float F = schlick(VdotH, reflectance) * specGain;
	// importance sample brdfs
	float r = (float) rand()/RAND_MAX;
	if (r > F) {
		return diffBrdf;
	}
	else{
		return specBrdf;
	}
}
