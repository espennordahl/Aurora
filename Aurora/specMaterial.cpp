//
//  specMaterial.cpp
//  Aurora
//
//  Created by Espen Nordahl on 12/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>

#include "specMaterial.h"
#include "blinn.h"

using namespace Aurora;

SpecMaterial::SpecMaterial( const Color &col, float exponent, int numSamples){
	color = col;
	brdf = new Blinn(color, exponent, numSamples);
}

Reference<Brdf> SpecMaterial::getBrdf( const Vector &Vn, const Vector &Nn, int thread ) {
	return brdf;
}
