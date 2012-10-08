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

MatteMaterial::MatteMaterial( std::string name, const Color &col, int numSamples){
	color = col;
	brdf = new Lambert(name + ":lambert", color, numSamples);
}

Reference<Brdf> MatteMaterial::getBrdf( const Vector &Vn, const Vector &Nn, const ShadingGeometry &shdGeo, int thread ) {
	return brdf;
}


void MatteMaterial::frameBegin(){
    brdf->frameBegin();
}

void MatteMaterial::frameEnd(){
    brdf->frameEnd();
}