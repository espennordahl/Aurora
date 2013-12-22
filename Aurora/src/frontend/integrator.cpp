//
//  intergrator.cpp
//  Aurora
//
//  Created by Espen Nordahl on 16/12/13.
//  Copyright (c) 2013 Espen Nordahl. All rights reserved.
//

#include "integrator.h"

#include "log.h"
#define lcontext LOG_Integrator

#include "intersection.h"
#include "brdf.h"
#include "attributeState.h"
#include "light.h"
#include "accelerationStructure.h"

using namespace Aurora;

Integrator::Integrator(RenderEnvironment *renderEnv):
m_render_environment(renderEnv)
{
    
}

inline float PowerHeuristic(int nf, float fPdf, int ng, float gPdf) {
    float f = nf * fPdf, g = ng * gPdf;
    return (f*f) / (f*f + g*g);
}

Integrator::IntegrationResult Integrator::integrateDirectLight(const LocalGeometry &lg, Light *light, Brdf *brdf, bxdfParameters *brdfParameters){
    Integrator::IntegrationResult result;
    result.alpha = 0;
    result.color = Color(0.f);
    result.raycount = 0;

    Sample3D lightSample = light->generateSample(lg.P, lg.Nn, brdf->m_integrationDomain);

        // sample light
    float costheta = dot(lg.Nn, lightSample.ray.direction);
    if (costheta >= 0. && lightSample.pdf > 0.) {
        float li = 1;
        ++result.raycount;
        if (m_render_environment->accelerationStructure->intersectBinary(&lightSample.ray))
            li = 0;
        result.color += lightSample.color *
        brdf->evalSampleWorld(lightSample.ray.direction, lg.Vn, lg.Nn, brdfParameters) *
        li * costheta / lightSample.pdf;
    }

    return result;
    
}

Integrator::IntegrationResult Integrator::integrateDirectMIS(const LocalGeometry &lg, Light *light, Brdf *brdf, bxdfParameters *brdfParameters){
    Integrator::IntegrationResult result;

    result.alpha = 0;
    result.color = Color(0.f);
    result.raycount = 0;

    
        // light sample
    Sample3D lightSample = light->generateSample(lg.P, lg.Nn, brdf->m_integrationDomain);
        // sample light
    float costheta = dot(lg.Nn, lightSample.ray.direction);
    if (costheta > 0. && lightSample.pdf > 0.) {
        float brdfPdf = brdf->pdf(lightSample.ray.direction, lg.Vn, lg.Nn, brdfParameters);
        if (brdfPdf > 0.) {
            float li = 1;
            ++result.raycount;
            if (m_render_environment->accelerationStructure->intersectBinary(&lightSample.ray))
                li = 0;
            if (li != 0) {
                float weight = PowerHeuristic(1, lightSample.pdf, 1, brdfPdf);
                result.color += lightSample.color * weight *
                brdf->evalSampleWorld(lightSample.ray.direction, lg.Vn, lg.Nn, brdfParameters) *
                li * costheta / lightSample.pdf;
            }
        }
    }
    
    
        // brdf sample
    Sample3D brdfSample = brdf->getSample(lg.Vn, lg.Nn, brdfParameters);
    brdfSample.ray.origin = lg.P;
    costheta = dot(lg.Nn, brdfSample.ray.direction);
    if (costheta > 0.) {
        float lightPdf = light->pdf(&brdfSample, lg.Nn, brdf->m_integrationDomain);
        if (lightPdf > 0. && brdfSample.pdf > 0.) {
            float li = 1;
            ++result.raycount;
            if (m_render_environment->accelerationStructure->intersectBinary(&brdfSample.ray))
                li = 0;
            if (li != 0) {
                float weight = PowerHeuristic(1, brdfSample.pdf, 1, lightPdf);
                result.color += brdfSample.color * weight *
                light->eval(brdfSample, lg.Nn) *
                costheta / brdfSample.pdf;
            }
        }
    }
    return result;
}

Integrator::IntegrationResult Integrator::integrateDirect(const LocalGeometry &lg, Brdf *brdf, bxdfParameters *brdfParameters){
    Integrator::IntegrationResult result;
    result.alpha = 0;
    result.color = Color(0.f);
    result.raycount = 0;
    
    int numLights = (int)m_render_environment->lights.size();
    std::vector<int> lightIndices;
    for(int i=0; i<numLights; ++i){
        if (m_render_environment->lights[i]->visible(lg.P, lg.Nn, brdf->m_integrationDomain)) {
            lightIndices.push_back(i);
        }
    }
    numLights = (int)lightIndices.size();
    
    if(numLights){
        Light* light = m_render_environment->lights[lightIndices[rand() % numLights]];
            // For diffuse lobes we only do light sampling
        if (brdf->m_brdfType == MatteBrdf) {
            result = integrateDirectLight(lg, light, brdf, brdfParameters);
        }
        
            // For specular lobes we do MIS
        else if (brdf->m_brdfType == SpecBrdf){
            result = integrateDirectMIS(lg, light, brdf, brdfParameters);
        }
            // for constant and mirror brdf we do nothing
        else {
            assert(brdf->m_brdfType == ConstantBrdf || brdf->m_brdfType == MirrorBrdf);
        }
        
        result.color *= (float)numLights;
    }
    
    return result;
}

Integrator::IntegrationResult Integrator::integrateCameraSample(Sample3D sample, int numSamples){

    Integrator::IntegrationResult result = IntegrationResult();
    result.alpha = 0;
    result.color = Color(0.f);
    result.raycount = 0;
    
    Intersection firstIsect;

        // find first intersection
    ++result.raycount;
    if (m_render_environment->accelerationStructure->intersect(&sample.ray, &firstIsect)) {
        result.alpha = 1.f;
        Color Lo = Color(0.f);
        LocalGeometry lg;
        
            // then find all the next vertices of the path
        for (int i=0; i < numSamples; i++) {
            float continueProbability = 1.f;
            Lo = Color(0.f);
            Sample3D currentSample = sample;
            Intersection isect = firstIsect;
            Color pathThroughput = Color(1.f);
            Brdf *currentBrdf;
            lg.Nn = Vector(0, 0, 1);
            bool mattePath = false;
            RayType rayType = CameraRay;
            
            int trueBounces = 0;
            for (int bounces = 0; ; ++bounces) {
                AttributeState *attrs = &m_render_environment->attributeState[isect.attributesIndex];
                isect.shdGeo.cameraToObject = attrs->cameraToObject;
                isect.shdGeo.objectToCamera = attrs->objectToCamera;
                attrs->material->runNormalShader(&isect.shdGeo);
                lg.Vn = normalize(-currentSample.ray.direction);
                lg.Nn = isect.shdGeo.Ns;
                lg.P = isect.hitP;
                if (rayType == DiffuseRay) {
                    mattePath = true;
                }
                
                    // emmision
                if (rayType == CameraRay || rayType == MirrorRay) {
                    Lo += pathThroughput * attrs->emmision;
                }
                
                    // with smooth normals there's a change we get normals
                    // on the "back side" of the ray.
                if (dot(lg.Vn, lg.Nn) <= 0.) {
                        // If so we force it to the front.
                    Vector tmp = cross(lg.Nn, lg.Vn);
                    lg.Nn = normalize(cross(lg.Vn, tmp));
                }
                
                    // sample lights
                BrdfState brdf_state = attrs->material->getBrdf(lg.Vn, lg.Nn, isect.shdGeo, mattePath);
                currentBrdf = brdf_state.brdf;
                bxdfParameters *brdf_parameters = brdf_state.parameters;
                assert(brdf_parameters != NULL);
                
                if (bounces < (*m_render_environment->globals)[MaxDepth]) {
                    Integrator::IntegrationResult directIntegration = integrateDirect(lg, currentBrdf, brdf_parameters);

                    result.raycount += directIntegration.raycount;
                    Lo += directIntegration.color * pathThroughput;
                }

                    // sample brdf
                currentSample = currentBrdf->getSample(lg.Vn, lg.Nn, brdf_parameters);
                delete brdf_parameters; //TODO: Lousy place to kill this
                if ( currentSample.pdf <= 0.f ) {
                    break;
                }

                if (currentBrdf->m_brdfType != MirrorBrdf) {
                    pathThroughput *= currentSample.color * dot(lg.Nn, currentSample.ray.direction) / currentSample.pdf;
                    if(bounces > (*m_render_environment->globals)[MinDepth]){
                        continueProbability *= currentSample.color.lum() * dot(lg.Nn, currentSample.ray.direction) / currentSample.pdf;
                    }
                    if (pathThroughput.isBlack()) {
                        break;
                    }
                    if (currentBrdf->m_brdfType == SpecBrdf) {
                        rayType = SpecularRay;
                    }
                    else {
                        rayType = DiffuseRay;
                    }
                }
                else {
                    pathThroughput *= currentSample.color;
                    if(bounces > (*m_render_environment->globals)[MinDepth]){
                        continueProbability *= currentSample.color.lum();
                    }
                    rayType = MirrorRay;
                }
                
                    // possibly terminate path
                if (bounces > (*m_render_environment->globals)[MinDepth]) {
                    if ((float) rand()/RAND_MAX > continueProbability) {
                        break;
                    }
                    pathThroughput /= continueProbability;
                }
                if (trueBounces == (*m_render_environment->globals)[MaxDepth]) {
                    break;
                }
                
                    // transform sample to world space
                    //currentSample.ray.direction = tangentToWorld(currentSample.ray.direction, Nn);
                currentSample.ray.origin = lg.P;
                
                    // if we're a mirror, we don't increment the bounce
                if (currentBrdf->m_brdfType == MirrorBrdf) {
                    --bounces;
                }
                trueBounces++;
                
                    // find next vertex
                ++result.raycount;
                if (!m_render_environment->accelerationStructure->intersect(&currentSample.ray, &isect)) {
                    if (rayType == MirrorRay) {
                        for (int i=0; i < m_render_environment->lights.size(); i++) {
                            Light* light = m_render_environment->lights[i];
                            if (light->lightType == type_envLight) {
                                Lo += light->eval(sample, sample.ray.direction) * pathThroughput;
                            }
                        }
                    }
                        // exit.
                    break;
                }
            }
            
            while(Lo.r > FIREFLY || Lo.g > FIREFLY || Lo.b > FIREFLY){
                LOG_WARNING("Reducing firefly: " << Lo);
                Lo *= 0.2f;
            }
            result.color += Lo/(float)numSamples;
        }
        
    } else { // camera ray miss
        for (int i=0; i < m_render_environment->lights.size(); i++) {
            Light* light = m_render_environment->lights[i];
            if (light->lightType == type_envLight) {
                    // multiplier is a hack to counter the div below
                result.color += light->eval(sample, sample.ray.direction);
                result.alpha = 1.;
            }
        }
    }
    
    return result;
}