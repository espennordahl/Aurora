//
//  kelemenMaterial.cpp
//  Aurora
//
//  Created by Espen Nordahl on 23/07/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>

#include "kelemenMaterial.h"
#include "cookTorrance.h"
#include "lambert.h"
#include "log.h"
#define lcontext LOG_KelemenMaterial
#include "shadingEngine.h"

#include <math.h>

using namespace Aurora;

#define SPECALBEDO_SAMPLES 50000
#define SPECALBEDO_ENTRIES 128

KelemenMaterial::KelemenMaterial( std::string name, RenderEnvironment *renderEnv, Color diffCol, Color specCol, 
                                 float _exponent, float _reflectance, int numSamples):
reflectance(_reflectance),
exponent(_exponent),
specGain(specCol.lum()), // TODO: I think this isn't used anymore

shader(KelemenShader(name + ":Shader", renderEnv, "", "", diffCol, specCol, _exponent, _reflectance, false, 0, 0, 0, 0, 0, 0))
{
        // settings

        // lobes
    specBrdf = new CookTorrance( name + ":CookTorrance", specCol, exponent, reflectance, std::max(numSamples, 32));
	diffBrdf = new Lambert( name + ":Lambert", diffCol, std::max(numSamples, 32));
    
        // shading
        // TODO : shdEngine should be initialized in frameBegin
    shdEngine = renderEnv->shadingEngine;
    shdEngine->registerShader(diffBrdf->name, &shader);	


        // albedo
    preCalcAlbedo();
}

#pragma mark -
#pragma mark pipeline

void KelemenMaterial::frameBegin(){
    specBrdf->frameBegin();
    diffBrdf->frameBegin();
}

void KelemenMaterial::frameEnd(){
    specBrdf->frameEnd();
    diffBrdf->frameEnd();
}

void KelemenMaterial::preCalcAlbedo(){
        // compute fresnel
    for (int i=0; i < SPECALBEDO_ENTRIES; i++) {        
        float albedo = 0;
        for (int j = 0; j < SPECALBEDO_SAMPLES; j++) {
            float r1 = (float) rand()/RAND_MAX;
            float r2 = (float) rand()/RAND_MAX;
            float r3 = (float) rand()/RAND_MAX;
            
            float z = cosf(atanf(exponent * sqrtf(-log(1.f-r1))));
            float r = sqrtf(fmax(0.f, 1.f - z*z));
            float phi = M_PI * 2 * r2;
            float x = r * cosf(phi);
            float y = r * sinf(phi);
                        
            Vector Hn = normalize(Vector(x,y,z));
            Vector Nn = Vector(0,0,1);
            
            z = (float)i/(SPECALBEDO_ENTRIES-1);
            r = sqrtf(fmax(0.f, 1.f-z*z));
            phi = r3 * 2 * M_PI;
            x = r * cosf(phi);
            y = r * sinf(phi);
            Vector Vn = normalize(Vector(x,y,z));
            
            if (dot(Hn, Vn) < 0.) {
                Hn = -Hn;
            }
            float vdoth = dot(Vn, Hn);
            if (vdoth > 0.001) {
                Vector Ln = (Hn * 2 * vdoth) - Vn;
                Ln = normalize(Ln);
                float hdotl = (dot(Hn,Ln));
                float F = schlick(hdotl, reflectance);
                float a = F * std::max(0.f,dot(Ln, Nn))/vdoth;
                albedo += a/(SPECALBEDO_SAMPLES/100);
            }
        }
        albedoTable.push_back(albedo/100);
    }
}

float KelemenMaterial::getAlbedo(float costheta){
    int i = clamp(floor((costheta * SPECALBEDO_ENTRIES)), 0., SPECALBEDO_ENTRIES-1);
    
    float valueA = albedoTable[i];
    float valueB = albedoTable[std::min(i+1,SPECALBEDO_ENTRIES-1)];
    
    float mix = costheta * SPECALBEDO_ENTRIES - (float)i;
    assert(valueA > 0. && valueB > 0.);
    assert(mix >= 0. && mix <= 1.0);
    float result = (1.-mix) * valueA + mix * valueB;
    return result;
}

Reference<Brdf> KelemenMaterial::getBrdf( const Vector &Vn, const Vector &Nn, const ShadingGeometry &shdGeo, bool mattePath, int thread ) {
    if (reflectance == 1.f) { // exception to get specular response only (for debug purposes)
        specBrdf->setWeight(1.f);
        return specBrdf;   
    }
    
        // get albedo
    float albedo = getAlbedo(clamp(dot(Nn, Vn), 0., 1.)); // TODO: should't need to clamp here
    if(albedo < 0.f && albedo > 1.f){
        LOG_ERROR("invalid albedo - outside of 0-1 range");
    }
        // importance sample brdfs
	float r = (float) rand()/RAND_MAX;
	if (r > albedo) {
        brdfParameters *params = shdEngine->getBrdfParameters(diffBrdf->name, shdGeo, thread);
        diffBrdf->setParameters(params, thread);
            // TODO: probably shouldn't free these here...
        free(params);
		return diffBrdf;
	}
	else{
        specBrdf->initRoughness(mattePath, thread); // TODO: specParams
		return specBrdf;
	}
}