//
//  triplanarTextureShader.h
//  Aurora
//
//  Created by Espen Nordahl on 16/06/13.
//  Copyright (c) 2013 Espen Nordahl. All rights reserved.
//

#ifndef Aurora_triplanarTextureShader_h
#define Aurora_triplanarTextureShader_h

#include "core.h"
#include "shader.h"
#include "texture2D.h"

#include <string.h>


namespace Aurora {
    template <class T> class TriplanarTextureShader : public Shader<T>{
    public:
        TriplanarTextureShader(std::string filename, float scale):
        m_texture(new Texture2D(filename)),
        m_scale(scale)
        {
            m_texture_name = filename;
        }
        
        T evaluate(const ShadingGeometry &shdGeo){
            float weights[3];
            float weightsum = 0.f;
            Vector N = normalize((*shdGeo.cameraToObject)(shdGeo.Ng));
            for (int i=0; i<3; ++i) {
                weights[i] = fabsf(N[i]);
                weights[i] -= 0.25;
                weights[i] = max(weights[i], 0.f);
                weightsum += weights[i];
            }
            for (int i=0; i<3; ++i) {
                weights[i] /= weightsum;
            }
            T result = T(0);
            Point P = (*shdGeo.cameraToObject)(shdGeo.P);
            P /= m_scale;
            for(int i=0; i < 3; ++i){
                if(weights[i] > 0){
                    float s = fabsf(fmodf(P[(i+1) % 3], 1.f));
                    float t = fabsf(fmodf(P[(i+2) % 3], 1.f));
                    result += m_texture->read(s, t, 0.f) * weights[i];
                }
            }
            
            return result;
        }
        
    private:
        float m_scale;
        std::string m_texture_name;
        Texture2D *m_texture;
    };

    template <> class TriplanarTextureShader<float> : public Shader<float>{
    public:
        TriplanarTextureShader(std::string filename, float scale):
        m_texture(new Texture2D(filename)),
        m_scale(scale)
        {
            m_texture_name = filename;
        }
        
        float evaluate(const ShadingGeometry &shdGeo){
            float weights[3];
            float weightsum = 0.f;
            for (int i=0; i<3; ++i) {
                weights[i] = fabsf(shdGeo.Ng[i]);
                weights[i] -= 0.2;
                weights[i] = max(weights[i], 0.f);
                weightsum += weights[i];
            }
            for (int i=0; i<3; ++i) {
                weights[i] /= weightsum;
            }
            float result = 0.f;
            Point P = (*shdGeo.cameraToObject)(shdGeo.P);
            P /= m_scale;
            for(int i=0; i < 3; ++i){
                if(weights[i] > 0){
                    float s = fabsf(fmodf(P[(i+1) % 3], 1.f));
                    float t = fabsf(fmodf(P[(i+2) % 3], 1.f));
                    result += m_texture->read(s, t, 0.f).r * weights[i];
                }
            }
            
            return result;
        }
        
    private:
        float m_scale;
        std::string m_texture_name;
        Texture2D *m_texture;
    };

    
}

#endif
