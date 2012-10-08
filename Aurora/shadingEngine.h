//
//  shadingEngine.h
//  Aurora
//
//  Created by Espen Nordahl on 7/10/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_shadingEngine_h
#define Aurora_shadingEngine_h

#include "core.h"
#include "brdf.h"
#include "shader.h"
#include "shaderCache.h"

#include <map>

namespace Aurora {
    class ShadingEngine{
    public:
        ShadingEngine();
        
        void registerShader(std::string brdfName, Shader *shader);
        brdfParameters *getBrdfParameters(std::string brdfName, const ShadingGeometry &shdGeo, int thread);
        
        std::map<std::string, Shader *> shaderRegister;
        ShaderCache cache;
    };
}


#endif
