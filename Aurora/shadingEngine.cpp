//
//  shadingEngine.cpp
//  Aurora
//
//  Created by Espen Nordahl on 7/10/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>

#include "shadingEngine.h"
#include "kelemenShader.h"
#include "log.h"
#define lcontext LOG_ShaderCache

using namespace Aurora;

ShadingEngine::ShadingEngine():
shaderRegister()
{
}

void ShadingEngine::registerShader(std::string brdfName, Shader *shader){
    LOG_DEBUG("Registering shader: " << brdfName);
    LOG_DEBUG("Current register size: " << shaderRegister.size());
    shaderRegister[brdfName] = shader;
}

brdfParameters *ShadingEngine::getBrdfParameters(std::string brdfName, const ShadingGeometry &shdGeo, int thread){
        // TODO: proper abstraction
    KelemenShader *shd = (KelemenShader*)shaderRegister[brdfName];
        // TODO: dont allocate stuff here..
    brdfParameters *params = new lambertParameters(shd->getLambertParams(shdGeo, thread));
    
    return params;
}
