//
//  cookTorrance.cpp
//  Aurora
//
//  Created by Espen Nordahl on 23/07/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>

#include "cookTorrance.h"
#include "shader.h"
#include "math.h"

#include <assert.h>

using namespace Aurora;

#define NORM_SAMPLES 100000
#define NORM_ENTRIES 128

CookTorrance::CookTorrance(std::string name, Color col, float _roughness, float _reflectance, int _numSamples, RenderEnvironment *renderEnv):
Brdf(name, renderEnv)
{
    brdfType = SpecBrdf;
    integrationDomain = Hemisphere;
    numSamples = _numSamples;
    for (int t=0; t < NUM_THREADS; t++) {
        generateSampleBuffer(0,t);
        generateSampleBuffer(1,t);
        generateSampleBuffer(2,t);
        reflectance[t] = _reflectance;
        color[t] = col;
        baseRoughness[t] = _roughness;
    }
//      Comment out to re bake
//    preCalcNormTable(); 
    normTable = new Texture2D("/Users/espennordahl/Documents/Aurora/cache/exrNormTable.exr");
}

#pragma mark -
#pragma mark Pipeline

void CookTorrance::frameBegin(){
    
}

void CookTorrance::frameEnd(){
    
}

void CookTorrance::setParameters(void *params, int thread){
    cookTorranceParameters *newParams = (cookTorranceParameters *) params;
    baseRoughness[thread] = newParams->roughness;
    reflectance[thread] = newParams->reflectance;
    color[thread] = newParams->specColor;
}


void CookTorrance::initRoughness(bool mattePath, int thread){
    if (mattePath == true && (*renderEnv->globals)[Caustics] == false) {
        roughness[thread] = baseRoughness[thread]/2 + 0.5;
    }
    else {
        roughness[thread] = baseRoughness[thread];
    }
}

void CookTorrance::preCalcNormTable(){
	OpenexrDisplay exrDisplay(NORM_ENTRIES, NORM_ENTRIES, "/Users/espennordahl/Documents/Aurora/cache/exrNormTable_v03.exr", renderEnv);
    for (int x = 0; x < NORM_ENTRIES; x++) {
        float roughness = clamp((float)x/NORM_ENTRIES, 0.01, 0.99);
        for (int y=0; y < NORM_ENTRIES; y++) {
            double norm = 0;
            for (int j = 0; j < NORM_SAMPLES; j++) {
                float r1 = (float) rand()/RAND_MAX;
                float r2 = (float) rand()/RAND_MAX;
                float r3 = (float) rand()/RAND_MAX;
                
                float _z = cosf(atan(roughness * sqrt(-logf(1.-r1))));
                float r = sqrtf(fmax(0.f, 1.f - _z*_z));
                float phi = M_PI * 2 * r2;
                float _x = r * cosf(phi);
                float _y = r * sinf(phi);
                
                Vector Hn = normalize(Vector(_x,_y,_z));
                
                Vector Nn = Vector(0,0,1);
                _z = (float)y/(NORM_ENTRIES-1);
                r = sqrtf(fmax(0.f, 1.f-_z*_z));
                phi = r3 * 2 * M_PI;
                _x = r * cosf(phi);
                _y = r * sinf(phi);
                Vector Vn = normalize(Vector(_x,_y,_z));
                float vdoth = dot(Vn, Hn);
                
                Vector Ln = normalize(Vector((Hn * 2 * vdoth) - Vn));
                
                float ldotn = dot(Ln,Nn);
                                
                if (vdoth > 0.001 && ldotn > 0.001) {
                    norm += (ldotn / vdoth)/((float)NORM_SAMPLES/1000);
                }   
            }
            norm /= 1000;
            assert(norm > 0.);
            exrDisplay.setPixel(x, y, Color(norm,0.5,0.5), 1.);
        }
    }
    exrDisplay.draw(NORM_ENTRIES);
}

float CookTorrance::getNormWeight(float costheta, float roughness){
    float r = (float)random() / RAND_MAX;
    float y = clamp(floor((costheta * NORM_ENTRIES) + (r-0.5)), 0., NORM_ENTRIES-1);
    float x = floorf(roughness * NORM_ENTRIES);
    float valueA = normTable->read((float)x/NORM_ENTRIES, (float)y/NORM_ENTRIES, 0.).r;
    float y2 = std::min(y + 1., NORM_ENTRIES-1.);
    float valueB = normTable->read((float)x/NORM_ENTRIES, (float)y2/NORM_ENTRIES, 0.).r;
    
    float mix = costheta * NORM_ENTRIES - (float)y;
    double result = 1./((1.-mix) * valueA + mix * valueB);
    assert(result > 0.);
    return result;
}


void CookTorrance::generateSampleBuffer(int i, int t){
    for (int j=0; j < numSamples; j++) {
        float offsetX = (float) rand()/RAND_MAX;
        float offsetY = (float) rand()/RAND_MAX;
        randomU[t][i].push_back((offsetX + (float)j)/numSamples);
        randomV[t][i].push_back((offsetY + (float)j)/numSamples);
    }
    for (int j=0; j < numSamples; j++) {
        int x = rand() % numSamples;
        float tmp = randomU[t][i][j];
        randomU[t][i][j] = randomU[t][i][x];
        randomU[t][i][x] = tmp;
        
        x = rand() % numSamples;
        tmp = randomV[t][i][j];
        randomV[t][i][j] = randomV[t][i][x];
        randomV[t][i][x] = tmp;
    }
}

inline float ctD(float roughness, float cosalpha, float tanalpha){
    float rough2 = roughness * roughness;
    return (1.f / (rough2 * M_PI * powf(cosalpha, 3.f))) * expf( -(tanalpha * tanalpha) / (rough2)) ;
}

Sample3D CookTorrance::getSample(const Vector &Vn, const Vector &Nn, int depth, int thread){
    
    float r1, r2;
    if (depth < 3) {
        if (randomU[thread][depth].size() == 0) {
            generateSampleBuffer(depth, thread);
        }
        r1 = randomU[thread][depth].back();
        randomU[thread][depth].pop_back();
        r2 = randomV[thread][depth].back();
        randomV[thread][depth].pop_back();
	}
    else {
        r1 = (float) rand()/RAND_MAX;
        r2 = (float) rand()/RAND_MAX;
    }
    
    float z = cosf(atanf(roughness[thread] * sqrtf(-log(1.-r1))));
    float r = sqrtf(fmax(0.f, 1.f - z*z));
    float phi = M_PI * 2 * r2;
    float x = r * cosf(phi);
    float y = r * sinf(phi);
    
    Vector dir = normalize(Vector(x,y,z));
    Vector Hn = normalize(tangentToWorld(dir, Nn));    
    float vdoth = dot(Vn, Hn);

//    if (vdoth < 0.) {
//        Hn = -Hn;
//        vdoth = dot(Vn, Hn);
//    }
    
    Vector Ln = normalize(Vector((Hn * 2 * vdoth) - Vn));
    
    float ldotn = dot(Ln,Nn);
    float vdotn = dot(Vn,Nn);
    float ndoth = dot(Nn,Hn);
    
//    if (vdoth < 0.) {
//        vdoth = -vdoth;
//    }
//    if (ldotn < 0.) {
//        ldotn = -ldotn;
//    }
    
    
    Color result = Color(0.f);
    float pdf = 1.f;

    if (vdoth > 0.001 && ldotn > 0.001 && vdotn > 0.001) {
            //float D = (1.f / (roughness[thread] * roughness[thread] * M_PI * powf(ndoth, 3.f))) * expf( -(tanf(acosf(ndoth)) * tanf(acosf(ndoth))) / (roughness[thread] * roughness[thread])) ;
        float D = ctD(roughness[thread], ndoth, tanf(acosf(ndoth)));
        float normalization = getNormWeight(dot(Nn, Vn), roughness[thread]);
        pdf = D / ( 4 * vdoth );
        if (isnan(D)) { // TODO: Make sure we don't get NaNs ahead of time.
            result = Color(0.);
            pdf = 0.f;
        }
        else {
            result =  color[thread] * normalization * D / ( 4 * vdoth * vdoth);
        }
        assert(result.lum() >= 0.);
        assert(pdf >= 0.);
        if (pdf > 100000) {
            pdf *= 0.1f;
            result *= 0.1f;
        }
        assert(pdf < 100000);
    }
    
    return Sample3D(Ray(Ln, Point(0.f), RAY_BIAS, 10000000.f), pdf, result);
}

float CookTorrance::pdf(const Vector &Ln, const Vector &Vn, const Vector Nn, int thread) const{

    Vector Hn = normalize(Ln + Vn);
    
    float vdoth = dot(Vn, Hn);
    float ndoth = dot(Nn, Hn);
    float ldotn = dot(Ln, Nn);
    float vdotn = dot(Vn, Nn);
    
    if (vdoth <= 0.001 || ndoth <= 0.001 || ldotn <= 0.001 || vdotn <= 0.001) {
        return 0;
    }    
    
//    float D = (1.f / (roughness[thread] * roughness[thread] * M_PI * powf(ndoth, 3.f))) * expf( -(tanf(acosf(ndoth)) * tanf(acosf(ndoth))) / (roughness[thread] * roughness[thread])) ;
    float D = ctD(roughness[thread], ndoth, tanf(acosf(ndoth)));

    if (isnan(D)) {
        D = 0.f;
    }
    assert(D >= 0.);
    return ( D ) / (4 * vdoth);
}


Color CookTorrance::evalSampleTangent(const Vector &Ln, const Vector &Vn, int thread){
    return Color(1);
}

Color CookTorrance::evalSampleWorld(const Vector &Ln, const Vector &Vn, const Vector &Nn, int thread){
    
    Vector Hn = normalize(Ln + Vn);
    float vdoth = dot(Vn, Hn);
//    float ldotn = dot(Ln, Nn);
    float vdotn = dot(Vn, Nn);
    float ndoth = dot(Hn, Nn);
    float ldotn = dot(Ln, Nn);
//    float ldoth = dot(Ln, Hn);
    
    float D = (1.f / (roughness[thread] * roughness[thread] * M_PI * powf(ndoth, 3.f))) * expf( -(tanf(acosf(ndoth)) * tanf(acosf(ndoth))) / (roughness[thread] * roughness[thread])) ;
    
    if (vdoth <= 0.001 || ldotn <= 0.001 || vdotn <= 0.001 || isnan(D)) {
        return Color(0.f);
    }
    float normalization = getNormWeight(dot(Nn, Vn), roughness[thread]);
    Color result = color[thread] * normalization * D / ( 4. * vdoth * vdoth );
    assert(result.lum() >= 0.);
    return result;
}