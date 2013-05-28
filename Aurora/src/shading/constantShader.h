//
//  constantShader.h
//  Aurora
//
//  Created by Espen Nordahl on 31/10/12.
//
//

#ifndef Aurora_constantShader_h
#define Aurora_constantShader_h

#include "core.h"
#include "shader.h"


namespace Aurora {
    template <typename T> class ConstantShader : public Shader<T> {
    public:
        ConstantShader(T _value): value(_value){};
        
        T evaluate(const ShadingGeometry &shdGeo){ return value; };
        
    private:
        T value;
    };
}

#endif
