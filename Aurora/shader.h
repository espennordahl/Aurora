//
//  shader.h
//  Aurora
//
//  Abstract:
//
//  The shading engine handles passing material properties 
//  to the bxdf's through the material. For now the parent
//  class is a bit of a dummy one.

#ifndef Aurora_shader_h
#define Aurora_shader_h

#include "renderEnvironment.h"
#include "brdf.h"
#include "shaderCache.h"
#include "core.h"
#include "frontEndObject.h"

#include <string.h>

namespace Aurora {    
    struct lambertParameters : brdfParameters{
        Color albedo;
    };
    
    struct cookTorranceParameters : brdfParameters{
        Color specColor;
        float roughness;
        float reflectance;
    };
    
    class Shader : public FrontEndObject {
    public:
        Shader(std::string name): FrontEndObject(name){};
        Shader(std::string name, RenderEnvironment *_renderEnv): FrontEndObject(name){ renderEnv = _renderEnv;};

            // the render environment is needed
            // to access prim vars, texture cache etc.
        RenderEnvironment *renderEnv;
    };
}

#endif
