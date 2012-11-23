//
//  texture2DShader.h
//  Aurora
//
//  Created by Espen Nordahl on 31/10/12.
//
//

#ifndef Aurora_texture2DShader_h
#define Aurora_texture2DShader_h

#include "core.h"
#include "shader.h"
#include "texture2D.h"

#include <string.h>


namespace Aurora {
    template <typename T> class Texture2DShader : public Shader<T>{
    public:
        Texture2DShader(std::string filename);
        
        T evaluate(const ShadingGeometry &shdGeo);
        
    private:
        std::string texName;
        Texture2D texture;
    };
}


#endif
