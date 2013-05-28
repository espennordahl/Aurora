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

#include "core.h"
#include "shadingGeometry.h"
#include "frontEndObject.h"

#include <string.h>

namespace Aurora {
    
    template <typename T> class Shader {
    public:        
        virtual T evaluate(const ShadingGeometry &shdGeo) = 0;
        virtual ~Shader(){};
    };
}

#endif
