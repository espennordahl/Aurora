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
        Color getColor(int index, const ShadingGeometry &shdGeo);
        float getFloat(int index, const ShadingGeometry &shdGeo);
        
        
        std::tr1::unordered_map<std::string, Material * > m_material_map;
        std::tr1::unordered_map<std::string, int > m_index_map;
        std::vector<Shader<Color> *> m_shd_map_color;
        std::vector<Shader<float> *> m_shd_map_float;
    };
}


#endif
