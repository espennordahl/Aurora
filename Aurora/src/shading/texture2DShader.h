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
    template <class T> class Texture2DShader : public Shader<T>{
    public:
            // TODO: How do I move the implementation to a .cpp file without
            // the linker complaining?
        Texture2DShader(std::string filename):
        texture(new Texture2D(filename))
        {
            texName = filename;
        }
        
        T evaluate(const ShadingGeometry &shdGeo){
                // TODO: Should probably floor elsewhere
            return texture->read(shdGeo.st.u - floor(shdGeo.st.u), shdGeo.st.v - floor(shdGeo.st.v), 0.);
        }
        
    private:
        std::string texName;
        Texture2D *texture;
    };
}


#endif
