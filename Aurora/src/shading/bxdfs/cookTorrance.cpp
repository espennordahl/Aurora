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

tbb::atomic<int> CookTorrance::m_halton_index = tbb::atomic<int>();

CookTorrance::CookTorrance(std::string name, RenderEnvironment *renderEnv):
Brdf(name, renderEnv)
{
    m_brdfType = SpecBrdf;
    m_integrationDomain = Hemisphere;
    m_sampler.init_faure();
}

#pragma mark -
#pragma mark Pipeline

void CookTorrance::frameBegin(){
    
}

void CookTorrance::frameEnd(){
    
}

inline float ctD(float roughness, float cosalpha, float tanalpha){
    const float rough2 = max(0.001f, roughness * roughness);
    const float exp = expf( -(tanalpha * tanalpha) / rough2);
    const float divisor = rough2 * M_PI * powf(cosalpha, 3.f);
    return exp / divisor;
}

Sample3D CookTorrance::getSample(const Vector &Vn, const Vector &Nn, bxdfParameters *parameters) const{
    CookTorranceParameters *params = static_cast<CookTorranceParameters*>(parameters);
    
#ifdef USE_HALTON
    const int i = m_halton_index.fetch_and_increment();
    float r1 = m_sampler.sample(SURFACE_DOMAIN_X, i);
    float r2 = m_sampler.sample(SURFACE_DOMAIN_Y, i);
#else
    float r1 = (float) rand()/RAND_MAX;
    float r2 = (float) rand()/RAND_MAX;
#endif
    
    float z = cosf(atanf(params->roughness * sqrtf(-log(1.-r1))));
    float r = sqrtf(fmax(0.f, 1.f - z*z));
    float phi = M_PI * 2 * r2;
    float x = r * cosf(phi);
    float y = r * sinf(phi);
    
    Vector dir = normalize(Vector(x,y,z));
    Vector Hn = normalize(tangentToWorld(dir, Nn));    
    float vdoth = dot(Vn, Hn);

    Vector Ln = normalize(Vector((Hn * 2 * vdoth) - Vn));
    
    float ldotn = dot(Ln,Nn);
    float vdotn = dot(Vn,Nn);
    float ndoth = dot(Nn,Hn);
    
    Color result = Color(0.f);
    float pdf = 1.f;

    if (vdoth > 0.0001){// && ldotn > 0.001 && vdotn > 0.001) {
        const float tanalpha = (ndoth > 0.999) ? 0. : tanf(acosf(ndoth));
        float D = ctD(params->roughness, ndoth, tanalpha);
        D = min(D,1000.f);
        pdf = D / ( 4 * vdoth );
        assert(!isnan(D));
        result = params->color * D / ( 4 * vdoth * vdoth);
        assert(result.lum() >= 0.);
        assert(pdf >= 0.);
        if (pdf > 100000) {
            pdf *= 0.01f;
            result *= 0.01f;
        }
        assert(pdf < 100000);
    }
    
    return Sample3D(Ray(Ln, Point(0.f), RAY_BIAS, 10000000.f), pdf, result);
}

float CookTorrance::pdf(const Vector &Ln, const Vector &Vn, const Vector Nn, bxdfParameters *parameters) const{
    CookTorranceParameters *params = static_cast<CookTorranceParameters*>(parameters);
    Vector Hn = normalize(Ln + Vn);
    
    float vdoth = dot(Vn, Hn);
    float ndoth = dot(Nn, Hn);
    float ldotn = dot(Ln, Nn);
    float vdotn = dot(Vn, Nn);
    
//    if (vdoth <= 0.001 || ndoth <= 0.001 || ldotn <= 0.001 || vdotn <= 0.001) {
//        return 0;
//    }    
    
    float D = ctD(params->roughness, ndoth, tanf(acosf(ndoth)));

    if (isnan(D)) {
        D = 0.f;
    }
    assert(D >= 0.);
    return ( D ) / (4 * vdoth);
}


Color CookTorrance::evalSampleTangent(const Vector &Ln, const Vector &Vn, bxdfParameters *parameters) const{
    assert(false);
    return Color(1);
}

Color CookTorrance::evalSampleWorld(const Vector &Ln, const Vector &Vn, const Vector &Nn, bxdfParameters *parameters) const{
    CookTorranceParameters *params = static_cast<CookTorranceParameters*>(parameters);
    
    Vector Hn = normalize(Ln + Vn);
    const float vdoth = dot(Vn, Hn);
    const float vdotn = dot(Vn, Nn);
    const float ndoth = dot(Hn, Nn);
    const float ldotn = dot(Ln, Nn);
    
    const float roughnessSquared = params->roughness * params->roughness;
    
    const float D = (1.f / (roughnessSquared * M_PI * powf(ndoth, 3.f))) * expf( -(tanf(acosf(ndoth)) * tanf(acosf(ndoth))) / roughnessSquared) ;
    
    if (vdoth <= 0.001 || ldotn <= 0.001 || vdotn <= 0.001 || isnan(D)) {
        return Color(0.f);
    }
    Color result = params->color * D / ( 4. * vdoth * vdoth );
    assert(result.lum() >= 0.);
    return result;
}
