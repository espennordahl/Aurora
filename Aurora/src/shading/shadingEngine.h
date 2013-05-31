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
#include "material.h"

#include <tr1/unordered_map>

namespace Aurora {
    class ShadingEngine{
    public:
        ShadingEngine();
        
        void registerMaterial(std::string materialName, Material * material);
        Material * getMaterial(std::string materialName);
        
        int registerShaderColor(const std::string &shdName, Shader<Color> *shader);
        int registerShaderFloat(const std::string &shdName, Shader<float> *shader);
        int getShaderIndex(const std::string &shdName);
//        int registerConstantColor(std::string attrName, Color c);
//        int registerConstantFloat(std::string attrName, float f);
        Color getColor(int index, const ShadingGeometry &shdGeo);
        float getFloat(int index, const ShadingGeometry &shdGeo);
        
        
        std::tr1::unordered_map<std::string, Material * > materialReg;
        std::tr1::unordered_map<std::string, int > indexMap;
        std::vector<Shader<Color> *> shdRegColor;
        std::vector<Shader<float> *> shdRegFloat;
//        std::vector<Color> colorCache;
//        std::vector<float> floatCache;
    };
}


#endif
