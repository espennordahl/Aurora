//
//  matteMaterial.cpp
//  Aurora
//
//  Created by Espen Nordahl on 07/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>

#include "matteMaterial.h"

using namespace Aurora;

MatteMaterial::MatteMaterial( const Color &col, int numSamples){
	color = col;
	brdf = new Lambert(color, numSamples);
}

Reference<Brdf> MatteMaterial::getBrdf( const Vector &Vn, const Vector &Nn ) const{
	return brdf;
}
