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
	brdf = new Lambert(name + ":lambert", renderEnv);
}

BrdfState MatteMaterial::getBrdf( const Vector &Vn, const Vector &Nn, const ShadingGeometry &shdGeo ) {
    bxdfParameters *params = new LambertParameters(m_renderEnv->shadingEngine->getColor(colorIndex, shdGeo));

    BrdfState state = {brdf, params};
	return state;
}

void MatteMaterial::frameBegin(){
    LOG_DEBUG(m_name << ": FrameBegin!");
    brdf->frameBegin();
}

void MatteMaterial::frameEnd(){
    brdf->frameEnd();
}