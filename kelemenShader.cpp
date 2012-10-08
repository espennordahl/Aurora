//
//  kelemenShader.cpp
//  Aurora
//
//  Created by Espen Nordahl on 21/09/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>

#include "kelemenShader.h"

#include "log.h"
#define lcontext LOG_KelemenShader


using namespace Aurora;


KelemenShader::KelemenShader(std::string name, RenderEnvironment *_renderEnv, 
                             std::string diffMapName, std::string roughnessMapName, 
                             const Color &diffColor, const Color &specColor, float roughness, float reflectance,
                             bool _useNoise, float _noiseFrequency, int _noiseLevels, float _noiseAmplitudeMod, float _noiseFreqMod,
                             const Color &_noiseColA, const Color &_noiseColB) :
Shader(name, _renderEnv)
{
    lambertParams.albedo = diffColor;
    cookTorranceParams.reflectance = reflectance;
    cookTorranceParams.roughness = roughness;
    cookTorranceParams.specColor = specColor;
    useNoise = _useNoise;
    noiseFrequency = _noiseFrequency;
    noiseLevels = _noiseLevels;
    noiseAmplitudeMod = _noiseAmplitudeMod;
    noiseFreqMod = _noiseFreqMod;
    noiseColA = _noiseColA;
    noiseColB = _noiseColB;

    if (diffMapName != "") {
        diffColorType = Texture;
        diffColorMap = new Texture2D("/Users/espennordahl/Documents/Aurora/textures/omg.exr");
    }
    else if (useNoise == true) {
        diffColorType = Noise;
        noise = Noise3D();
    }
    else{
        diffColorType = Constant;
    }
    
    shdCache = new ShaderCache[NUM_THREADS];
}

void KelemenShader::frameBegin(){
    
}

void KelemenShader::frameEnd(){
    
}

cookTorranceParameters KelemenShader::getCookTorranceParams(const ShadingGeometry &shdGeo){
    return cookTorranceParams;
}


lambertParameters KelemenShader::getLambertParams(const ShadingGeometry &shdGeo, int threadNum){
    if (diffColorType == Constant) {
        
    }
    else if (diffColorType == Texture) {
        lambertParams.albedo = diffColorMap->read(shdGeo.st.u, shdGeo.st.v, 0);
    }
    else { 
        Point P = shdGeo.P *noiseFrequency; 
        P = (*shdGeo.cameraToObject)(P);
        float n = 0;
        float amp = 0.75;
        float freq = 1;
        for (int i=0; i < noiseLevels; i++) {
            n += noise.eval(P*freq) * amp;
            amp *= noiseAmplitudeMod;
            freq *= noiseFreqMod;
        }
        n = clamp(n * 0.5 + 0.5, 0., 1.); // normalize
        lambertParams.albedo = noiseColA * (1.-n) + noiseColB * n;
        return lambertParams;
        
    }
    return lambertParams;
}