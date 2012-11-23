//
//  matteMaterial.cpp
//  Aurora
//
//  Created by Espen Nordahl on 07/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>

#include "matteMaterial.h"
#include "log.h"
#define lcontext LOG_MatteMaterial

using namespace Aurora;

MatteMaterial::MatteMaterial( std::string name, const Color &col, int numSamples, RenderEnvironment *renderEnv):
Material(name, renderEnv)
{
	color = col;
	brdf = new Lambert(name + ":lambert", color, numSamples, renderEnv);
}

Reference<Brdf> MatteMaterial::getBrdf( const Vector &Vn, const Vector &Nn, const ShadingGeometry &shdGeo, bool mattePath, int thread ) {
	return brdf;
}


void MatteMaterial::frameBegin(){
    LOG_DEBUG("FrameBegin!");
    brdf->frameBegin();
}

void MatteMaterial::frameEnd(){
    brdf->frameEnd();
}