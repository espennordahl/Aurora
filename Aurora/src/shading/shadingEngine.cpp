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

void ShadingEngine::registerMaterial(std::string materialName, Material * material){
    LOG_DEBUG("Registering material: " << materialName);
    m_material_map[materialName] = material;
}

Material * ShadingEngine::getMaterial(std::string materialName){
    return m_material_map[materialName];
}


int ShadingEngine::registerShaderColor(const std::string &shdName, Shader<Color> *shader){
    LOG_DEBUG("Registering color shader: " << shdName);
    m_shd_map_color.push_back(shader);
    int index = (int)m_shd_map_color.size()-1;
    m_index_map[shdName] = index;
    return index;
}

int ShadingEngine::registerShaderFloat(const std::string &shdName, Shader<float> *shader){
    LOG_DEBUG("Registering float shader: " << shdName);
    m_shd_map_float.push_back(shader);
    int index = (int)m_shd_map_float.size()-1;
    m_index_map[shdName] = index;
    return index;
}

void ShadingEngine::replaceShaderColor(int index, Shader<Color> *shader){
    m_shd_map_color[index] = shader;
}

void ShadingEngine::replaceShaderFloat(int index, Shader<float> *shader){
    m_shd_map_float[index] = shader;
}


int ShadingEngine::getShaderIndex(const std::string &shdName){
    if (m_index_map.find(shdName) == m_index_map.end()) {
        LOG_ERROR("Shader not found in ShadingEngine: " << shdName);
    }
    return m_index_map[shdName];
}

Color ShadingEngine::getColor(int index, const ShadingGeometry &shdGeo){
    return m_shd_map_color[index]->evaluate(shdGeo);
}

float ShadingEngine::getFloat(int index, const ShadingGeometry &shdGeo){
    return m_shd_map_float[index]->evaluate(shdGeo);
}