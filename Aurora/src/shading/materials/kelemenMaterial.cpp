//
//  kelemenMaterial.cpp
//  Aurora
//
//  Created by Espen Nordahl on 23/07/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>

#include <ImfArray.h>
#include <ImfRgba.h>
#include <ImfInputFile.h>

#include "kelemenMaterial.h"
#include "cookTorrance.h"
#include "shaders.h"
#include "lambert.h"
#include "log.h"
#define lcontext LOG_KelemenMaterial
#include "shadingEngine.h"

#include <math.h>
#include <sstream>

using namespace Aurora;

KelemenMaterial::KelemenMaterial( std::string name, RenderEnvironment *renderEnv,
                                 int _diffColIndex, int _specColIndex,
                                 int _roughnessIndex, float _reflectance,
                                 int normalIndex
                                 ):
Material(name, renderEnv, normalIndex),
reflectance(_reflectance),
roughnessIndex(_roughnessIndex),
specColorIndex(_specColIndex),
diffColorIndex(_diffColIndex)
{
        // lobes
    specBrdf = new CookTorrance( name + ":CookTorrance", renderEnv);
	diffBrdf = new Lambert( name + ":Lambert", renderEnv);
    
        // Reflectance is non varying for now. TODO
    ConstantShader<float> *refShd = new ConstantShader<float>(reflectance);
    reflectanceIndex = renderEnv->shadingEngine->registerShaderFloat(name + ":specReflectance", refShd);
    
        // Load albedo cache
        // TODO: Lerp between for specAlbedo
    stringstream ss;
    float modrefl = roundf(reflectance * (SPECALBEDO_REFLECTANCE_ENTRIES-1))/(SPECALBEDO_REFLECTANCE_ENTRIES-1.f);
    ss << modrefl;//add number to the stream
    std::string texName = "/Users/espennordahl/Documents/Aurora/cache/exrBeckmanAlbedoTable_" + ss.str() + "_v02.exr";
    Imf::InputFile file (texName.c_str());
    Imf::Array2D<half> pixels;
    Imath::Box2i dw = file.header().dataWindow();
    int width = dw.max.x - dw.min.x + 1;
    int height = dw.max.y - dw.min.y + 1;
    pixels.resizeErase(height, width);
    Imf::FrameBuffer frameBuffer;
    frameBuffer.insert ("R",	// name
                        Imf::Slice (Imf::HALF,	// type
                        (char *) (&pixels[0][0] -	// base
                        dw.min.x -
                        dw.min.y * width),
                        sizeof (pixels[0][0]) * 1,	// xStride
                        sizeof (pixels[0][0]) * width,	// yStride
                        1, 1,	// x/y sampling
                        0.0));	// fillValue
    file.setFrameBuffer (frameBuffer);
    file.readPixels (dw.min.y, dw.max.y);
    
    albedoTable.resize(width);
    for (int x=0; x<width; ++x) {
        albedoTable[x].resize(height);
        for(int y=0; y<height; ++y){
            albedoTable[x][y] = pixels[y][x];
        }
    }
    
    assert(width == SPECALBEDO_COSTHETA_ENTRIES);
    assert(height == SPECALBEDO_ROUGHNESSS_ENTRIES);
}

#pragma mark -
#pragma mark pipeline

void KelemenMaterial::frameBegin(){
    LOG_DEBUG("FrameBegin!");
        // Run this to write out a new cache
    /*
    float oldRefl = reflectance;
    for (int i=0; i < SPECALBEDO_REFLECTANCE_ENTRIES; i++){
        reflectance = (float)i/(SPECALBEDO_REFLECTANCE_ENTRIES-1);
        preCalcAlbedo();
        for (int x=0; x < albedoTable.size(); x++) {
            albedoTable[x].clear();
        }
        albedoTable.clear();
    }
    reflectance = oldRefl;
    */
    specBrdf->frameBegin();
    diffBrdf->frameBegin();
}

void KelemenMaterial::frameEnd(){
    specBrdf->frameEnd();
    diffBrdf->frameEnd();
}


    // Used for building tables of specAlbedo and baking them into exr files
void KelemenMaterial::preCalcAlbedo(){
    LOG_DEBUG("Buidling abledo tables");
        // we need one talbe per roughness value to support varying roughness
    for (int r=0; r < SPECALBEDO_ROUGHNESSS_ENTRIES; r++){
        LOG_DEBUG("Table calc " << 100 * (float)r/SPECALBEDO_ROUGHNESSS_ENTRIES << "% complete");
        std::vector<float> newTable;
        albedoTable.push_back(newTable);
        float roughness = std::max(0.00001f,(float)r/(SPECALBEDO_ROUGHNESSS_ENTRIES-1.f));
        
            // each table computes the specAlbedo for a range of cosThetas
        for (int i=0; i < SPECALBEDO_COSTHETA_ENTRIES; i++) {
            double albedo = 0;
            for (int j = 0; j < SPECALBEDO_SAMPLES; j++) {
                    // sample beckman distribution
                float r1 = (float) rand()/RAND_MAX;
                float r2 = (float) rand()/RAND_MAX;
                float r3 = (float) rand()/RAND_MAX;
                
                float z = cosf(atanf(roughness * sqrtf(-log(1.f-r1))));
                float r = sqrtf(fmax(0.f, 1.f - z*z));
                float phi = M_PI * 2 * r2;
                float x = r * cosf(phi);
                float y = r * sinf(phi);
                            
                Vector Hn = normalize(Vector(x,y,z));
                Vector Nn = Vector(0,0,1);
                
                    // costheta
                z = std::max(0.0001f,(float)i/(SPECALBEDO_COSTHETA_ENTRIES-1));
                r = sqrtf(fmax(0.f, 1.f-z*z));
                phi = r3 * 2 * M_PI;
                x = r * cosf(phi);
                y = r * sinf(phi);
                Vector Vn = normalize(Vector(x,y,z));

                    // run fresnel on our sample if it's visible to
                    // the light and the viewer
                if (dot(Hn, Vn) < 0.) {
                    Hn = -Hn;
                }
                float vdoth = dot(Vn, Hn);
                if (vdoth > 0.000001) {
                    Vector Ln = (Hn * 2 * vdoth) - Vn;
                    Ln = normalize(Ln);
                    float hdotl = (dot(Hn,Ln));
                    float F = schlick(hdotl, reflectance);
                        // albedo as per kelemen et al
                    double a = F * std::max(0.f,dot(Ln, Nn))/vdoth;
                        // accumulate albedo
                    albedo += a/((float)SPECALBEDO_SAMPLES/100);
                }
            }
            albedoTable[r].push_back(albedo/100.f);
        }
    }
    stringstream ss;//create a stringstream
    ss << reflectance;//add number to the stream
    std::string texName = "/Users/espennordahl/Documents/Aurora/cache/exrBeckmanAlbedoTable_" + ss.str() + "_v02.exr";
    OpenexrDisplay exrDisplay(SPECALBEDO_COSTHETA_ENTRIES, SPECALBEDO_ROUGHNESSS_ENTRIES, texName, m_renderEnv);
    for (int x=0; x < SPECALBEDO_COSTHETA_ENTRIES; x++) {
        for (int y=0; y < SPECALBEDO_ROUGHNESSS_ENTRIES; y++) {
            exrDisplay.setPixel(x, y, Color(albedoTable[y][x]), 1.);
        }
    }
    exrDisplay.draw(SPECALBEDO_ROUGHNESSS_ENTRIES);
}

float KelemenMaterial::getAlbedo(float costheta, float roughness){
/*    Commented: Reading using the precalc table. Now using texture cache (see below)
 int i = clamp(floor((costheta * SPECALBEDO_COSTHETA_ENTRIES)), 0., SPECALBEDO_COSTHETA_ENTRIES-1);
    int roughnessA = clamp(floor(roughness/SPECALBEDO_ROUGHNESSS_ENTRIES), 0, SPECALBEDO_ROUGHNESSS_ENTRIES-1);
    int roughnessB = roughnessA+1;

        // first roughness lookup
    float valueA = albedoTable[roughnessA][i];
    float valueB = albedoTable[roughnessA][std::min(i+1,SPECALBEDO_COSTHETA_ENTRIES-1)];
    
    float mix = costheta * SPECALBEDO_COSTHETA_ENTRIES - (float)i;
    assert(valueA >= 0. && valueB >= 0.);
    assert(mix >= 0. && mix <= 1.0);
    float resultRoughnessA = (1.-mix) * valueA + mix * valueB;

        // second roughness lookup
    valueA = albedoTable[roughnessB][i];
    valueB = albedoTable[roughnessB][std::min(i+1,SPECALBEDO_COSTHETA_ENTRIES-1)];
    float resultRoughnessB = (1.-mix) * valueA + mix * valueB;

*/
    
    int i = floor(costheta * (SPECALBEDO_COSTHETA_ENTRIES-1));
    int roughnessA = floor(roughness * (SPECALBEDO_ROUGHNESSS_ENTRIES-1));
    int roughnessB = max(roughnessA+1, SPECALBEDO_ROUGHNESSS_ENTRIES-1);
    
        // first roughness lookup
    float valueA = albedoTable[i][roughnessA];
    float valueB = albedoTable[std::min(i+1, SPECALBEDO_COSTHETA_ENTRIES-1)][roughnessA];
    
    float mix = costheta * (SPECALBEDO_COSTHETA_ENTRIES-1) - i;
    assert(valueA >= 0. && valueB >= 0.);
    assert(mix >= 0. && mix <= 1.0);
    float resultRoughnessA = (1.-mix) * valueA + mix * valueB;
    
        // second roughness lookup
    valueA = albedoTable[i][roughnessB];
    valueB = albedoTable[std::min(i+1,SPECALBEDO_COSTHETA_ENTRIES-1)][roughnessB];
    float resultRoughnessB = (1.-mix) * valueA + mix * valueB;

        // mix lookups
    float roughnessMix = roughness * (SPECALBEDO_ROUGHNESSS_ENTRIES-1) - roughnessA;
    return (1.-roughnessMix) * resultRoughnessA + roughnessMix * resultRoughnessB;
}

BrdfState KelemenMaterial::getBrdf( const Vector &Vn, const Vector &Nn, const ShadingGeometry &shdGeo, bool mattePath) {

    CookTorranceParameters *ctParams = new CookTorranceParameters(
        m_renderEnv->shadingEngine->getColor(specColorIndex, shdGeo),
        m_renderEnv->shadingEngine->getFloat(roughnessIndex, shdGeo),
        reflectance);
    
    if (mattePath) {
        ctParams->roughness = ctParams->roughness * (1.-CAUSTIC_BLUR) + CAUSTIC_BLUR;
    }

    if (ctParams->reflectance < 1.f) { // exception to get specular response only (for debug purposes)
            // get albedo
        float albedo = getAlbedo(clamp(dot(Nn, Vn), 0., 1.), ctParams->roughness); // TODO: should't need to clamp here
        assert(albedo <= 1. && albedo >= 0.);
        
            // importance sample brdfs
        float r = (float) rand()/RAND_MAX;
        if (r > albedo) {
            delete ctParams;
            LambertParameters *params = new LambertParameters(
            m_renderEnv->shadingEngine->getColor(diffColorIndex, shdGeo)
            );
            BrdfState state = {diffBrdf, (bxdfParameters*)params};
            return state;
        }
    }
    
    BrdfState state = {specBrdf, (bxdfParameters*) ctParams};
    return state;
}
