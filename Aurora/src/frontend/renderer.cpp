//
//  renderer.cpp
//  Aurora
//
//  Created by Espen Nordahl on 30/06/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "renderer.h"
#include "shapes.h"
#include "auroraObject.h"
#include "materials.h"
#include "embreeAccelerator.h"
#include "embreeMesh.h"
#include "lights.h"
#include "shadingEngine.h"

#include "log.h"
#define lcontext LOG_Renderer

#include <iostream>
#include <math.h>

#include <tbb/parallel_for.h>
#include <tbb/task.h>
#include <tbb/task_group.h>

#include "jsonParser.h"

using namespace Aurora;

Renderer::Renderer( char *file ){
    filename = std::string(file);
}

void Renderer::render(){
    
    time_t parseBegin;
	time(&parseBegin);

    parseSceneDescription();
	
    	// Time
	time_t parseEnd;
	time(&parseEnd);
	int totalTime = difftime(parseEnd, parseBegin);
    
	LOG_INFO("Total parsing time: "
             << floor(totalTime/60/60) << " h "
             << floor((totalTime/60) % 60) << " min "
             << totalTime % 60 << " sec.");

    time_t envBegin;
	time(&envBegin);
    
	buildRenderEnvironment();
    	// Time
	time_t envEnd;
	time(&envEnd);
	totalTime = difftime(envEnd, envBegin);
    
	LOG_INFO("Total time building render env: "
             << floor(totalTime/60/60) << " h "
             << floor((totalTime/60) % 60) << " min "
             << totalTime % 60 << " sec.");

    
	renderImageTBB();
	
	outputStats();
}

void Renderer::parseSceneDescription(){
    LOG_INFO("*************************************");
	LOG_INFO("Parsing scene description.");
    
    renderEnv = RenderEnvironment();
    renderEnv.shadingEngine = new ShadingEngine();
    renderEnv.globals =  new AuroraGlobals;
    renderEnv.stringGlobals = new StringMap;
    setDefaultOptions(renderEnv.globals);

        // open file
    JsonParser parser = JsonParser(filename, &renderEnv);
    parser.parseScene(NULL);
    objects = parser.getObjects();
    lights = parser.getLights();
    
	LOG_INFO("Done parsing scene description.");	
    LOG_INFO("*************************************\n");
}

void Renderer::buildRenderEnvironment(){
    LOG_INFO("*************************************");
	LOG_INFO("Building render environment.");
    
        // preFrame
    for (u_int32_t i=0; i < objects.size(); i++) {
        objects[i]->frameBegin();
    }
    
        // camera
    renderCam = new Camera((*renderEnv.globals)[FieldOfView], (*renderEnv.globals)[ResolutionX], (*renderEnv.globals)[ResolutionY], (*renderEnv.globals)[PixelSamples]);
    
	if (!objects.size()) {
		LOG_ERROR("No objects found.");
	}
	
        // acceleration structure
    AccelerationStructure *accel;
    
    int numObjects = (int)objects.size();
    int numLights = (int)lights.size();
    attrs = new AttributeState[numObjects + numLights];
    EmbreeMesh mesh;
    for (int i=0; i < numObjects; i++) {
        mesh.appendTriangleMesh(objects.back()->m_shape, i);
        attrs[i].material = objects.back()->m_material;
        attrs[i].emmision = Color(0.);
        objects.pop_back();
    }
    for (int i=0; i < numLights; i++) {
        if (lights[i]->lightType != type_envLight) {
            mesh.appendTriangleMesh(lights[i]->shape(), i + numObjects);
        }
        Material * black = new ConstantMaterial("Not in use - lightsource", Color(0.f), &renderEnv);
        attrs[i + numObjects].material = black;
        attrs[i + numObjects].emmision = lights[i]->emission();
    }
    accel = new EmbreeAccelerator(mesh, attrs);
    
	renderEnv.accelerationStructure = accel;
	renderEnv.attributeState = attrs;
	renderEnv.lights = lights;
	renderEnv.envLight = envLight;
	renderEnv.renderCam = renderCam;
    	
	LOG_INFO("Done building render environment.");
    LOG_INFO("*************************************\n");
}

inline float PowerHeuristic(int nf, float fPdf, int ng, float gPdf) {
    float f = nf * fPdf, g = ng * gPdf;
    return (f*f) / (f*f + g*g);
}


class IntegrateParallel {
public:
    IntegrateParallel(int width,
                      int height,
                      int num_samples,
                      int sample_index,
                      RenderEnvironment *render_environment,
                      OpenexrDisplay *display):
    m_width(width), m_height(height), m_num_samples(num_samples), m_sample_index(sample_index),
    m_render_environment(render_environment), m_display(display)
    {}
    
    void operator()(const tbb::blocked_range<size_t>& r) const{
        for(size_t i=r.begin(); i!=r.end(); ++i){
            int x = i % m_width;
            int y = (int)i / m_width;
            Sample2D current_sample_2d = Sample2D(x, y);
            
            Sample3D sample = m_render_environment->renderCam->convertSample(current_sample_2d, m_sample_index);
            Color Lo = Color(0.f);
            float alpha = 0.f;
            Intersection firstIsect;
            
                // find first intersection
            if (m_render_environment->accelerationStructure->intersect(&sample.ray, &firstIsect)) {
                alpha = 1.f;
                
                    // then find all the next vertices of the path
                for (int i=0; i < m_num_samples; i++) {
                    Sample3D currentSample = sample;
                    Intersection isect = firstIsect;
                    Color pathThroughput = Color(1.f);
                    Brdf *currentBrdf;
                    Vector Nn = Vector(0, 0, 1);
                    bool mattePath = false;
                    RayType rayType = CameraRay;
                    
                    int trueBounces = 0;
                    for (int bounces = 0; ; ++bounces) {
                        AttributeState *attrs = &m_render_environment->attributeState[isect.attributesIndex];
                        isect.shdGeo.cameraToObject = attrs->cameraToObject;
                        isect.shdGeo.objectToCamera = attrs->objectToCamera;
                        Vector Vn = normalize(-currentSample.ray.direction);
                        Nn = normalize(isect.hitN);
                        Point orig = isect.hitP;
                        if (rayType == DiffuseRay) {
                            mattePath = true;
                        }
                        
                            // emmision
                        if (rayType == CameraRay || rayType == MirrorRay) {
                            Lo += pathThroughput * attrs->emmision;
                        }
                        
                            // with smooth normals there's a change we get normals
                            // on the "back side" of the ray.
                        if (dot(Vn,Nn) <= 0.) {
                                // If so we force it to the front.
                            Vector tmp = cross(Nn, Vn);
                            Nn = normalize(cross(Vn, tmp));
                        }
                        
                            // sample lights
                        BrdfState brdf_state = attrs->material->getBrdf(Vn, Nn, isect.shdGeo);
                        currentBrdf = brdf_state.brdf;
                        bxdfParameters *brdf_parameters = brdf_state.parameters;
                        
                        if (bounces < (*m_render_environment->globals)[MaxDepth]) {
                            int numLights = (int)m_render_environment->lights.size();
                            Light* currentLight = m_render_environment->lights[rand() % numLights];
                            
                                // For diffuse samples we don't need MIS
                            if (currentBrdf->m_brdfType == MatteBrdf) {
                                
                                Sample3D lightSample = currentLight->generateSample(orig, Nn, currentBrdf->m_integrationDomain);
                                    // sample light
                                float costheta = dot(Nn, lightSample.ray.direction);
                                if (costheta >= 0. && lightSample.pdf > 0.) {
                                    float li = 1;
                                    if (m_render_environment->accelerationStructure->intersectBinary(&lightSample.ray))
                                        li = 0;
                                    Lo += lightSample.color *
                                    currentBrdf->evalSampleWorld(lightSample.ray.direction, Vn, Nn, brdf_parameters) *
                                    li * costheta * pathThroughput * (float)numLights / lightSample.pdf;
                                }
                            }
                            
                                // Specular lobes MIS
                            else {
                                
                                    // light sample
                                Sample3D lightSample = currentLight->generateSample(orig, Nn, currentBrdf->m_integrationDomain);
                                    // sample light
                                float costheta = dot(Nn, lightSample.ray.direction);
                                if (costheta > 0. && lightSample.pdf > 0.) {
                                    float brdfPdf = currentBrdf->pdf(lightSample.ray.direction, Vn, Nn, brdf_parameters);
                                    if (brdfPdf > 0.) {
                                        float li = 1;
                                        if (m_render_environment->accelerationStructure->intersectBinary(&lightSample.ray))
                                            li = 0;
                                        if (li != 0) {
                                            float weight = PowerHeuristic(1, lightSample.pdf, 1, brdfPdf);
                                            Lo += lightSample.color * weight *
                                            currentBrdf->evalSampleWorld(lightSample.ray.direction, Vn, Nn, brdf_parameters) *
                                            li * costheta * pathThroughput * (float)numLights / lightSample.pdf;
                                        }
                                    }
                                }
                                
                                
                                    // brdf sample
                                Sample3D brdfSample = currentBrdf->getSample(Vn, Nn, brdf_parameters);
                                brdfSample.ray.origin = orig;
                                costheta = dot(Nn, brdfSample.ray.direction);
                                if (costheta > 0.) {
                                    float lightPdf = currentLight->pdf(&brdfSample, Nn, currentBrdf->m_integrationDomain);
                                    if (lightPdf > 0. && brdfSample.pdf > 0.) {
                                        float li = 1;
                                        if (m_render_environment->accelerationStructure->intersectBinary(&brdfSample.ray))
                                            li = 0;
                                        if (li != 0) {
                                            float weight = PowerHeuristic(1, brdfSample.pdf, 1, lightPdf);
                                            Lo += brdfSample.color * weight *
                                            currentLight->eval(brdfSample, Nn) *
                                            costheta * pathThroughput * (float)numLights / brdfSample.pdf;
                                        }
                                    }
                                }
                            }
                            
                        }
                            // sample brdf
                        currentSample = currentBrdf->getSample(Vn, Nn, brdf_parameters);
                        delete brdf_parameters; //TODO: Lousy place to kill this
                        if ( currentSample.pdf <= 0.f ) {
                            break;
                        }
                        if (currentBrdf->m_brdfType != MirrorBrdf) {
                            pathThroughput *= currentSample.color * dot(Nn, currentSample.ray.direction) / currentSample.pdf;
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
                            rayType = MirrorRay;
                        }
                            // possibly terminate path here
                        if (bounces > (*m_render_environment->globals)[MinDepth]) {
                            float continueProbability = min((float)MAX_ROULETTE, pathThroughput.lum()*10);
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
                        currentSample.ray.origin = orig;
                        
                            // if we're a mirror, we don't increment the bounce
                        if (currentBrdf->m_brdfType == MirrorBrdf) {
                            --bounces;
                        }
                        trueBounces++;
                            // find next vertex
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
                }
                
            } else { // camera ray miss
                for (int i=0; i < m_render_environment->lights.size(); i++) {
                    Light* light = m_render_environment->lights[i];
                    if (light->lightType == type_envLight) {
                        // multiplier is a hack to counter the div below
                        Lo += light->eval(sample, sample.ray.direction) * (float)m_num_samples;
                        alpha = 1.;
                    }
                }
            }
            
            m_display->appendValue(x, y, Lo/(float)m_num_samples, alpha);
        }
    }
    
private:
    int m_width;
    int m_height;
    int m_num_samples;
    int m_sample_index;
    RenderEnvironment *m_render_environment;
    OpenexrDisplay *m_display;
};

class DrawTask
{
    OpenexrDisplay *m_driver;
    int m_height;
public:
    DrawTask(OpenexrDisplay *driver, int height):m_driver(driver), m_height(height){}
    void operator()()
    {
        m_driver->draw(m_height);
    }
};

void Renderer::renderImageTBB(){
    LOG_INFO("*************************************");
	LOG_INFO("Rendering image.");
	
        // start render timer
	time(&renderTime);
	
        // set up buffer and sampler
	int width = renderCam->getWidthSamples();
	int height = renderCam->getHeightSamples();
	int multisamples = renderCam->getPixelSamples();
    std::string fn = (*renderEnv.stringGlobals)["fileName"];
    if(fn == ""){
        LOG_ERROR("Render output filename is blank");
    }
    displayDriver = new OpenexrDisplay(width, height, fn, &renderEnv);
    displayDriver->draw(height);
    tbb::task_group group;
    for(int i=0; i<multisamples; ++i){
        tbb::parallel_for( tbb::blocked_range<size_t>(0,width*height),
                          IntegrateParallel(width,
                                            height,
                                            (*renderEnv.globals)[LightSamples],
                                            i,
                                            &renderEnv,
                                            displayDriver
                                            )
                          );
        group.wait();
        group.run(DrawTask(displayDriver, height));
        LOG_INFO("Render progress: " << 100 * (i+1)/(float)multisamples << "%");
    }
    group.wait();
}


void Renderer::outputStats(){
    LOG_INFO("*************************************");
	LOG_INFO("Outputting statistics.");
	
    LOG_INFO("Renderer version: Aurora v" << VERSION);
    
	// Time
	time_t renderEnd;
	time(&renderEnd);
	int totalTime = difftime(renderEnd, renderTime);
    
	LOG_INFO("Total render time: " 
             << floor(totalTime/60/60) << " h " 
             << floor((totalTime/60) % 60) << " min " 
             << totalTime % 60 << " sec.");
    std::string stringTime =    intToString(floor(totalTime/60/60)) + " h " +
                                intToString(floor((totalTime/60) % 60)) + " min "+
                                intToString(totalTime % 60) + " sec.";
    displayDriver->addMetadata(std::string("renderTime"), stringTime);
    displayDriver->draw(displayDriver->height());

	LOG_INFO("Done outputting statistics.");
    LOG_INFO("*************************************\n");
}