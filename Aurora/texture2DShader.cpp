//
//  texture2DShader.cpp
//  Aurora
//
//  Created by Espen Nordahl on 31/10/12.
//
//

#include "texture2DShader.h"
#include "log.h"
#define lcontext LOG_Texture2DShd

using namespace Aurora;

template <typename T>
Texture2DShader<T>::Texture2DShader(std::string filename){
    texName = filename;
    texture = Texture2D(filename);
    LOG_ERROR("INIT!");
}

template <typename T>
T Texture2DShader<T>::evaluate(const ShadingGeometry &shdGeo){
    return texture.read(shdGeo.st.u, shdGeo.st.v, 0.);
}