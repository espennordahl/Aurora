//
//  metalMaterial.cpp
//  Aurora
//
//  Created by Espen Nordahl on 7/06/13.
//  Copyright (c) 2013 Espen Nordahl. All rights reserved.
//

#include "metalMaterial.h"

#include "shadingEngine.h"

#include "log.h"
#define lcontext LOG_MetalMaterial

using namespace Aurora;

MetalMaterial::MetalMaterial( std::string name, RenderEnvironment *renderEnv,
                             int roughnessIndexA, int roughnessIndexB,
                             int mixIndex, int reflectanceIndex,
                             int gainIndex, int normalIndex):
Material(name, renderEnv, normalIndex),
m_reflectanceIndex(reflectanceIndex),
m_roughnessIndexA(roughnessIndexA),
m_roughnessIndexB(roughnessIndexB),
m_mixIndex(mixIndex),
m_gainIndex(gainIndex)
{
	m_brdf = new CookTorrance(name + ":cookTorranceA", renderEnv);
}

BrdfState MetalMaterial::getBrdf( const Vector &Vn, const Vector &Nn, const ShadingGeometry &shdGeo, bool mattePath) {
    float mix = m_renderEnv->shadingEngine->getFloat(m_mixIndex, shdGeo);
    float r = rand()/(float)RAND_MAX;
    Color reflectance = m_renderEnv->shadingEngine->getColor(m_reflectanceIndex, shdGeo);
    Color gain = m_renderEnv->shadingEngine->getColor(m_gainIndex, shdGeo);
    Color spec_color;
    float costheta = dot(Vn, Nn);
    spec_color.r = schlick(costheta, reflectance.r);
    spec_color.g = schlick(costheta, reflectance.g);
    spec_color.b = schlick(costheta, reflectance.b);
    if(r > mix){
        bxdfParameters *params = new CookTorranceParameters(spec_color * gain,
                                                            mattePath ?
                                                            m_renderEnv->shadingEngine->getFloat(m_roughnessIndexA, shdGeo)
                                                            * (1.-CAUSTIC_BLUR) + CAUSTIC_BLUR :
                                                            m_renderEnv->shadingEngine->getFloat(m_roughnessIndexA, shdGeo),
                                                            1.);
        BrdfState state = {m_brdf, params};
        return state;

    } else {
        bxdfParameters *params = new CookTorranceParameters(spec_color * gain,
                                                            mattePath ?
                                                            m_renderEnv->shadingEngine->getFloat(m_roughnessIndexB, shdGeo)
                                                            * (1.-CAUSTIC_BLUR) + CAUSTIC_BLUR :
                                                            m_renderEnv->shadingEngine->getFloat(m_roughnessIndexB, shdGeo),
                                                            1.);
        BrdfState state = {m_brdf, params};
        return state;
    }
}

void MetalMaterial::frameBegin(){
    LOG_DEBUG(m_name << ": FrameBegin!");
    m_brdf->frameBegin();
}

void MetalMaterial::frameEnd(){
    m_brdf->frameEnd();
}