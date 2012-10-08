//
//  kelemenShader.h
//  Aurora
//
//  Created by Espen Nordahl on 21/09/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_kelemenShader_h
#define Aurora_kelemenShader_h

#include "shader.h"
#include "texture2D.h"
#include "noise3D.h"
#include <string.h>

namespace Aurora { 
    
    enum channelType{
        Texture,
        Constant,
        Noise
    };
    
    class KelemenShader : public Shader{
    public:
        KelemenShader(std::string name, RenderEnvironment *_renderEnv, 
                      std::string diffMapName, std::string roughnessMapName, 
                      const Color &diffColor, const Color &specColor, float roughness, float reflectance,
                      bool useNoise, float noiseFrequency, int noiseLevels, float noiseAmplitudeMod, float noiseFreqMod,
                      const Color &noiseColA, const Color &noiseColB);
        
        void frameBegin();
        void frameEnd();
        
        cookTorranceParameters getCookTorranceParams(const ShadingGeometry &shdGeo);
        lambertParameters getLambertParams(const ShadingGeometry &shdGeo, int threadNum);
        
    private:
        cookTorranceParameters cookTorranceParams;
        lambertParameters lambertParams;
        Texture2D *diffColorMap;
        Texture2D *roughnessMap;
        Noise3D noise;
        bool useNoise;
        float noiseFrequency;
        int noiseLevels;
        float noiseAmplitudeMod;
        float noiseFreqMod;
        Color noiseColA;
        Color noiseColB;
        channelType diffColorType;
        channelType roughnessType;
        channelType reflectanceType;
        ShaderCache *shdCache;
//        texture2D *reflectanceMap;
    };
}

#endif
