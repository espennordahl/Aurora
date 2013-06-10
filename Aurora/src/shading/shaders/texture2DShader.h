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
        Texture2DShader(std::string filename):
        m_texture(new Texture2D(filename))
        {
            m_texture_name = filename;
        }
        
        T evaluate(const ShadingGeometry &shdGeo){
                // TODO: Should probably floor elsewhere
            return m_texture->read(shdGeo.st.u - floor(shdGeo.st.u), 1.f- (shdGeo.st.v - floor(shdGeo.st.v)), 0.);
        }
        
    private:
        std::string m_texture_name;
        Texture2D *m_texture;
    };
    
    template <> class Texture2DShader <float> : public Shader<float>{
    public:
        Texture2DShader(std::string filename):
        m_texture(new Texture2D(filename))
        {
            m_texture_name = filename;
        }
        
        float evaluate(const ShadingGeometry &shdGeo){
                // TODO: Should probably floor elsewhere
            return m_texture->read(shdGeo.st.u - floor(shdGeo.st.u), 1.f- (shdGeo.st.v - floor(shdGeo.st.v)), 0.).r;
        }
        
    private:
        std::string m_texture_name;
        Texture2D *m_texture;
    };

}


#endif
