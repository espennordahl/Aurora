//
//  material.cpp
//  Aurora
//
//  Created by Espen Nordahl on 25/07/13.
//  Copyright (c) 2013 Espen Nordahl. All rights reserved.
//

#include "material.h"

#include "shadingEngine.h"

#include "log.h"
#define lcontext LOG_Material

using namespace Aurora;

Material::Material(std::string name, RenderEnvironment *renderEnv, int normalindex):
FrontEndObject(name, renderEnv),
m_normals_index(normalindex){
    if(normalindex != UNSET_SHADER_INDEX){
        LOG_INFO("Assigning normal shader to material: " << name);
    }
};


void Material::runNormalShader(ShadingGeometry *shdGeo){
    if(m_normals_index != UNSET_SHADER_INDEX){
        Color map = m_renderEnv->shadingEngine->getColor(m_normals_index, *shdGeo);
        Vector Ns = tangentToWorld(Vector(map.r, map.g, map.b), shdGeo->Ng);
        if(!Ns.isZero()){
            shdGeo->Ns = normalize(Ns);
            assert(dot(shdGeo->Ns, shdGeo->Ng) > 0.);
        } else {
            shdGeo->Ns = shdGeo->Ng;
        }
    } else {
        shdGeo->Ns = shdGeo->Ng;
    }
}

void Material::applyAttributeChange(const AttributeChange &change){
    LOG_ERROR("Unable to apply attribute change.");
}