//
//  lambert.cpp
//  Aurora
//
//  Created by Espen Nordahl on 06/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>

#include "material.h" // TODO: just here to get lambert struct..
#include "lambert.h"
#include "shader.h"
#include "log.h"
#define lcontext LOG_Lambert

using namespace Aurora;

tbb::atomic<int> Lambert::m_halton_index = tbb::atomic<int>();

Lambert::Lambert(std::string name, RenderEnvironment *renderEnv) :
Brdf(name, renderEnv)
{
    m_brdfType = MatteBrdf;
    m_integrationDomain = Hemisphere;
    m_sampler.init_faure();
}

#pragma mark -
#pragma mark Pipeline

void Lambert::frameBegin(){
    
}

void Lambert::frameEnd(){
    
}