//
//  matteMaterial.cpp
//  Aurora
//
//  Created by Espen Nordahl on 07/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>

#include "shadingEngine.h"

#include "matteMaterial.h"
#include "log.h"
#define lcontext LOG_MatteMaterial

using namespace Aurora;

MatteMaterial::MatteMaterial( std::string name, int diffColorIndex, int numSamples, RenderEnvironment *renderEnv):
Material(name, renderEnv),
colorIndex(diffColorIndex)
{
	brdf = new Lambert(name + ":lambert", Color(1,0,1), numSamples, renderEnv);
}

Reference<Brdf> MatteMaterial::getBrdf( const Vector &Vn, const Vector &Nn, const ShadingGeometry &shdGeo, bool mattePath, int thread ) {
    lambertParameters params = {
        renderEnv->shadingEngine->getColor(colorIndex, shdGeo)
    };
    brdf->setParameters(&params, thread);
	return brdf;
}


void MatteMaterial::frameBegin(){
    LOG_DEBUG(name << ": FrameBegin!");
    brdf->frameBegin();
}

void MatteMaterial::frameEnd(){
    brdf->frameEnd();
}