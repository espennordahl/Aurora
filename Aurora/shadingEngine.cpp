//
//  shadingEngine.cpp
//  Aurora
//
//  Created by Espen Nordahl on 7/10/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>

#include "shadingEngine.h"
#include "log.h"
#define lcontext LOG_ShaderCache

using namespace Aurora;

ShadingEngine::ShadingEngine()
{
}

void ShadingEngine::registerMaterial(std::string materialName, Reference<Material> material){
    LOG_DEBUG("Registering material: " << materialName);
    materialReg[materialName] = material;
}

Reference<Material> ShadingEngine::getMaterial(std::string materialName){
    return materialReg[materialName];
}


int ShadingEngine::registerShaderColor(const std::string &shdName, Shader<Color> *shader){
    LOG_DEBUG("Registering shader: " << shdName);
    shdRegColor.push_back(shader);
    int index = (int)shdRegColor.size()-1;
    indexMap[shdName] = index;
    return index;
}

int ShadingEngine::registerShaderFloat(const std::string &shdName, Shader<float> *shader){
    LOG_DEBUG("Registering shader: " << shdName);
    shdRegFloat.push_back(shader);
    int index = (int)shdRegFloat.size()-1;
    indexMap[shdName] = index;
    return index;
}

int ShadingEngine::getShaderIndex(const std::string &shdName){
    if (indexMap.find(shdName) == indexMap.end()) {
        LOG_ERROR("Shader not found in ShadingEngine: " << shdName);
    }
    return indexMap[shdName];
}


//int ShadingEngine::registerConstantColor(std::string attrName, Color c){
//    LOG_DEBUG("Registering constant color: " << attrName);
//    colorCache.push_back(c);
//    return (int)colorCache.size()-1;
//}
//
//int ShadingEngine::registerConstantFloat(std::string attrName, float f){
//    LOG_DEBUG("Registering constant float: " << attrName);
//    floatCache.push_back(f);
//    return (int)floatCache.size()-1;
//}

Color ShadingEngine::getColor(int index, const ShadingGeometry &shdGeo){
    return shdRegColor[index]->evaluate(shdGeo);
}

float ShadingEngine::getFloat(int index, const ShadingGeometry &shdGeo){
    return shdRegFloat[index]->evaluate(shdGeo);
}