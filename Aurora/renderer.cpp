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
#include "dummyAccelerationStructure.h"
#include "uniformGridAccelerator.h"
#include "kdTreeAccelerator.h"
#include "embreeAccelerator.h"
#include "adaptiveRndSampler2D.h"
#include "embreeMesh.h"
#include "lights.h"
#include "shadingEngine.h"
#include "parsing.h"
#include "log.h"
#define lcontext LOG_Renderer

#include <iostream>
#include <fstream>
#include <pthread.h>
#include <math.h>

#include "json.h"

using namespace Aurora;

Renderer::Renderer( char *file ){
    filename = std::string(file);
    setDefaultOptions(&globals);
}

void Renderer::render(){
	parseSceneDescription();
	
	buildRenderEnvironment();
	
	renderImage();
	
	outputStats();
}

void Renderer::parseSceneDescription(){
    LOG_INFO("*************************************");
	LOG_INFO("Parsing scene description.");
    
    renderEnv = RenderEnvironment();
    renderEnv.shadingEngine = new ShadingEngine();

        // open file
    
	Json::Value deserializeRoot;
	Json::Reader reader;
    ifstream sceneDescription( filename.c_str() );
	if ( !reader.parse(sceneDescription, deserializeRoot) )
		LOG_ERROR("Couldn't parse file: " << filename);
	else{
        LOG_INFO("Parsing json file: " << filename);
        
            // get globals first, then camera, then objects
        if( deserializeRoot.size() > 0 ) {
                // Options
            for( Json::ValueIterator itr = deserializeRoot.begin() ; itr != deserializeRoot.end() ; itr++ ) {
                std::string key = itr.key().asString();
                LOG_DEBUG("Parsing scene key: " << key);
                if (key == "objects") {
                    ;
                }
                else if (key == "options") {
                    parseGlobals(*itr, &globals);
                }
                else if (key == "renderCam"){
                    ;
                }
                else {
                    LOG_WARNING("Unable to parse Scene attribute: " << key);
                }
            }
        }
        cameraTransform = new Transform();
        if( deserializeRoot.size() > 0 ) {
                // Camera
            for( Json::ValueIterator itr = deserializeRoot.begin() ; itr != deserializeRoot.end() ; itr++ ) {
                std::string key = itr.key().asString();
                LOG_DEBUG("Parsing scene key: " << key);
                if (key == "objects") {
                    ;
                }
                else if (key == "options") {
                    ;
                }
                else if (key == "renderCam"){
                    parseCamera(*itr, &globals, cameraTransform);
                }
                else {
                    LOG_WARNING("Unable to parse Scene attribute: " << key);
                }
            }
        }
        
        if( deserializeRoot.size() > 0 ) {
                // Objects
            for( Json::ValueIterator itr = deserializeRoot.begin() ; itr != deserializeRoot.end() ; itr++ ) {
                std::string key = itr.key().asString();
                LOG_DEBUG("Parsing scene key: " << key);
                if (key == "objects") {
                    parseObject(*itr, &renderEnv, objects, lights, cameraTransform, globals);
                }
                else if (key == "options") {
                    ;
                }
                else if (key == "renderCam"){
                    ;
                }
                else {
                    LOG_WARNING("Unable to parse Scene attribute: " << key);
                }
            }
        }
	}
    
	LOG_INFO("Done parsing scene description.");	
    LOG_INFO("*************************************\n");
}

void Renderer::buildRenderEnvironment(){
    LOG_INFO("*************************************");
	LOG_INFO("Building render environment.");
        
        // camera
    AdaptiveRndSampler2D *cameraSampler = new AdaptiveRndSampler2D();
    renderCam = Camera(globals[FieldOfView], globals[ResolutionX], globals[ResolutionY], globals[PixelSamples], cameraSampler);
    
	if (!objects.size()) {
		LOG_ERROR("No objects found.");
	}
	
    AccelerationStructure *accel;
    
    if (globals[AccelStructure] != ACCEL_EMBREE) {
        int numObjects = (int)objects.size();
        attrs = new AttributeState[numObjects + lights.size()];
        std::vector<RenderableTriangle> renderable;
        for (int i=0; i < numObjects; i++) {
            objects[i]->makeRenderable(renderable, attrs, i);
        }
        for (int i=0; i < lights.size(); i++) {
            lights[i]->makeRenderable(renderable, attrs, numObjects + i);
        }
        
    //	AccelerationStructure *accel = new DummyAccelerationStructure(renderable);
    //	AccelerationStructure *accel = new UniformGridAccelerator(renderable);
        accel = new KdTreeAccelerator(renderable, KD_INTERSECTCOST, KD_TRAVERSECOST, KD_EMPTYBONUS, globals[KD_MaxLeaf], globals[KD_MaxDepth]);
    }
    else {
        int numObjects = (int)objects.size();
        int numLights = (int)lights.size();
        attrs = new AttributeState[numObjects + numLights];
        EmbreeMesh mesh;
        for (int i=0; i < numObjects; i++) {
            mesh.appendTriangleMesh(objects[i]->shape, i);
            attrs[i].material = objects[i]->material;
            attrs[i].emmision = Color(0.);
        }
        for (int i=0; i < numLights; i++) {
            if (lights[i]->lightType != type_envLight) {
                mesh.appendTriangleMesh(lights[i]->shape(), i + numObjects);
            }
            Reference<Material> black = new MatteMaterial("Not in use - Should be EDF", Color(0),1);
            attrs[i + numObjects].material = black;
            attrs[i + numObjects].emmision = lights[i]->emission();
        }
        accel = new EmbreeAccelerator(mesh, attrs);
    }
    

	
	renderEnv.accelerationStructure = accel;
	renderEnv.attributeState = attrs;
	renderEnv.lights = lights;
	renderEnv.envLight = envLight;
	renderEnv.renderCam = &renderCam;
    renderEnv.globals = &globals;
	
	LOG_INFO("Done building render environment.");
    LOG_INFO("*************************************\n");
}


struct threadargs{
	RenderEnvironment *renderEnv;
	Sample2D *sampleBuffer;
	int numSamples;
    int threadNum;
	float renderProgress;
};

void *integrateThreaded( void *threadid );

void Renderer::renderImage(){
    LOG_INFO("*************************************");
	LOG_INFO("Rendering image.");
	
	// start render timer
	time_t renderBegin;
	time(&renderBegin);
	
	// set up buffer and sampler
	// TODO: This might be better if it was done at parsing time	
	int width = renderCam.getWidthSamples();
	int height = renderCam.getHeightSamples();
	int multisamples = renderCam.getPixelSamples();
	OpenexrDisplay display(width, height, "/Users/espennordahl/aurora2.exr");
	
	
	// render camera samples
	Sample2D currentSample;
	PixelRegion region;
	
	renderCam.generateSamples();
	
	// start draw timer
	time_t beginDraw, endDraw;
	time(&beginDraw);
	time(&renderTime);
	renderProgress = 0.f;
	
	std::vector<std::vector < int > > multisampleBuffer;
	multisampleBuffer.resize(width);
	for (int i=0; i < width; i++) {
		multisampleBuffer[i].resize(height);
	}
	
	for (int i=0; i < width; i++) {
		for (int j=0; j < height; j++) {
			multisampleBuffer[i][j] = 1;
		}
	}
	int nRenderedSamples = 0;	
	
	pthread_t threads[NUM_THREADS];	
	struct threadargs td[NUM_THREADS];
	void *status;	
	int rc;			
	std::vector<Sample2D> renderedSamples;

	while (true) {
		// THREADS
		Sample2D *sampleBuffer;
		sampleBuffer = new Sample2D[SAMPLES_PER_THREAD];

		int accumulatedSamples = 0;
		int threadsSpawned = 0;
		bool finalSamples = true;

		while (renderCam.nextSample(&currentSample)) {
			sampleBuffer[accumulatedSamples] = currentSample;
			accumulatedSamples++;
			
			if (accumulatedSamples == SAMPLES_PER_THREAD) {
				// spawn new thread
				td[threadsSpawned].renderEnv = &renderEnv;
				td[threadsSpawned].renderProgress = renderProgress;
				Sample2D *tmpBuffer;
				tmpBuffer = new Sample2D[SAMPLES_PER_THREAD];
				memcpy(tmpBuffer, sampleBuffer, SAMPLES_PER_THREAD * sizeof(Sample2D));
				td[threadsSpawned].sampleBuffer = tmpBuffer;
				td[threadsSpawned].numSamples = SAMPLES_PER_THREAD;
                td[threadsSpawned].threadNum = threadsSpawned;
				
				rc = pthread_create(&threads[threadsSpawned], NULL, integrateThreaded, (void *)&td[threadsSpawned]);
				
				if (rc){
					cout << "Error:unable to create thread," << rc << endl;
					exit(-1);
				}
				
				threadsSpawned++;
				if (threadsSpawned == NUM_THREADS){
					finalSamples = false;
					break;
				}
				accumulatedSamples = 0;
			}
		}

 // if we have the last samples, we need to create a final thread
		if (finalSamples) {
			
			td[threadsSpawned].renderEnv = &renderEnv;
			td[threadsSpawned].renderProgress = renderProgress;
			td[threadsSpawned].numSamples = accumulatedSamples;
			Sample2D *tmpBuffer;
			tmpBuffer = new Sample2D[SAMPLES_PER_THREAD];
			memcpy(tmpBuffer, sampleBuffer, SAMPLES_PER_THREAD * sizeof(Sample2D));
			td[threadsSpawned].sampleBuffer = tmpBuffer;
			
			rc = pthread_create(&threads[threadsSpawned], NULL, integrateThreaded, (void *)&td[threadsSpawned]);
			
			if (rc){
				cout << "Error:unable to create thread," << rc << endl;
				exit(-1);
			}
			threadsSpawned++;
		}
		free(sampleBuffer);
		
		renderedSamples.clear();
		// join threads
		for (int i = 0; i < threadsSpawned; i++) {
			rc = pthread_join(threads[i], &status);
			if (rc){
				cout << "Error:unable to join," << rc << endl;
				exit(-1);
			}
			for (int j=0; j < td[i].numSamples; j++) {
				Sample2D s = td[i].sampleBuffer[j];
				renderedSamples.push_back(s);
			}
			free(td[i].sampleBuffer);
		}

//		renderedSamples.push_back(Sample2D());
		
		// STORE SAMPLES
		for (int nSample = 0; nSample < renderedSamples.size(); nSample++){
			//Sample2D drawingSample = renderedSamples[nSample];
			Sample2D drawingSample = renderedSamples[nSample];
			region = drawingSample.region;
            if (drawingSample.col.hasNaNs()) {
                drawingSample.col = Color(0.f);
            }
			// each sample is potentially stored in multiple pixels
			for (int i=region.xMin; i < region.xMax; i++) {
				for (int j=region.yMin; j < region.yMax; j++) {
					if (multisampleBuffer[i][j] == 1) {
						display.setPixel(i, j, drawingSample.col, drawingSample.alpha);					
					}
					else{
						Color oldCol;
						float oldAlpha;
						display.getPixel(i, j, &oldCol, &oldAlpha);
						float sampleNum = multisampleBuffer[i][j];
						float weight = 1.f/sampleNum;
						display.setPixel(i, j, oldCol*(1-weight) + drawingSample.col*weight, 
								   oldAlpha*(1-weight) + drawingSample.alpha*weight);
					}
				}
			}
			
			multisampleBuffer[drawingSample.x][drawingSample.y]++;
			time(&endDraw);
			if (difftime(endDraw, beginDraw) > DRAW_DELAY){
				renderProgress = (float)nRenderedSamples/(width*height*multisamples);
				LOG_INFO("Rendering : " << renderProgress * 100.f << "% complete.");
				time(&beginDraw);
				display.draw(height);
			}
			nRenderedSamples++;
		}
		if (finalSamples) {
			break;
		}

	}
	
	display.draw(height);

	
	LOG_INFO("Done rendering image.");
    LOG_INFO("*************************************\n");
}

inline float PowerHeuristic(int nf, float fPdf, int ng, float gPdf) {
     float f = nf * fPdf, g = ng * gPdf;
     return (f*f) / (f*f + g*g);
}

void *integrateThreaded( void *threadid ){
	
	struct threadargs *args;
	
	args = (struct threadargs*) threadid;
	float renderProgress = args->renderProgress;
	RenderEnvironment *renderEnv = args->renderEnv;
	Sample2D *samples = args->sampleBuffer;
	int numCameraSamples = args->numSamples;
    int threadNum = args->threadNum;
	
	for (int nSample = 0; nSample < numCameraSamples; nSample++) {
		
		Sample3D sample = renderEnv->renderCam->convertSample(samples[nSample]);
		Color Lo = Color(0.f);
		float alpha = 0.f;
		Intersection firstIsect;
		
		// find first intersection
		if (renderEnv->accelerationStructure->intersect(&sample.ray, &firstIsect)) {
			alpha = 1.f;
			
			// then find all the next vertices of the path
			for (int i=0; i < (*renderEnv->globals)[LightSamples]; i++) {
				Sample3D currentSample = sample;
				Intersection isect = firstIsect;
				Color pathThroughput = Color(1.f);
				Reference<Brdf> currentBrdf;	
				Vector Nn = Vector(0, 0, 1);
                bool mattePath = false;
                RayType rayType = CameraRay;
                
				int trueBounces = 0;
				for (int bounces = 0; ; ++bounces) {
					AttributeState *attrs = &renderEnv->attributeState[isect.attributesIndex];
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
					currentBrdf = attrs->material->getBrdf(Vn, Nn, isect.shdGeo, mattePath, threadNum);
					
					if (bounces < (*renderEnv->globals)[MaxDepth]) {
						int numLights = (int)renderEnv->lights.size();
						Reference<Light> currentLight = renderEnv->lights[rand() % numLights];
                        
                        
                            // For diffuse samples we don't to MIS
                        if (currentBrdf->brdfType == MatteBrdf) {

                            Sample3D lightSample = currentLight->generateSample(orig, Nn, currentBrdf->integrationDomain, bounces, threadNum);
                            // sample light
                            float costheta = dot(Nn, lightSample.ray.direction);
                            if (costheta >= 0. && lightSample.pdf > 0.) {
                                float li = 1;
                                if (renderEnv->accelerationStructure->intersectBinary(&lightSample.ray))
                                    li = 0;
                                Lo += lightSample.color * 
                                currentBrdf->evalSampleWorld(lightSample.ray.direction, Vn, Nn, threadNum) * li * costheta * pathThroughput * (float)numLights / lightSample.pdf;
                            }
                        }
                        
                            // Specular lobes MIS
                        else {
                            
                                // light sample
                            Sample3D lightSample = currentLight->generateSample(orig, Nn, currentBrdf->integrationDomain, bounces, threadNum);
                                // sample light
                            float costheta = dot(Nn, lightSample.ray.direction);
                            if (costheta > 0. && lightSample.pdf > 0.) {
                                float brdfPdf = currentBrdf->pdf(lightSample.ray.direction, Vn, Nn, threadNum);
                                if (brdfPdf > 0.) {
                                    float li = 1;
                                    if (renderEnv->accelerationStructure->intersectBinary(&lightSample.ray))
                                        li = 0;
                                    if (li != 0) {
                                        float weight = PowerHeuristic(1, lightSample.pdf, 1, brdfPdf);
                                        Lo += lightSample.color * weight *
                                        currentBrdf->evalSampleWorld(lightSample.ray.direction, Vn, Nn, threadNum) * li *
                                        costheta * pathThroughput * (float)numLights / lightSample.pdf;
                                    }
                                }
                            }


                                // brdf sample
                            Sample3D brdfSample = currentBrdf->getSample(Vn, Nn, bounces, threadNum);
                            brdfSample.ray.origin = orig;
                            costheta = dot(Nn, brdfSample.ray.direction);
                            if (costheta > 0.) {
                                float lightPdf = currentLight->pdf(&brdfSample, Nn, currentBrdf->integrationDomain);
                                if (lightPdf > 0. && brdfSample.pdf > 0.) {
                                    float li = 1;
                                    if (renderEnv->accelerationStructure->intersectBinary(&brdfSample.ray))
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
					currentSample = currentBrdf->getSample(Vn, Nn, bounces, threadNum);
					if ( currentSample.pdf <= 0.f ) {
						break;
					}
                    if (currentBrdf->brdfType != MirrorBrdf) {
                        pathThroughput *= currentSample.color * dot(Nn, currentSample.ray.direction) / currentSample.pdf;
                        if (pathThroughput.isBlack()) {
                            break;
                        }
                        if (currentBrdf->brdfType == SpecBrdf) {
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
					if (bounces > (*renderEnv->globals)[MinDepth]) {
						float continueProbability = min((float)MAX_ROULETTE, pathThroughput.lum()*10);
						if ((float) rand()/RAND_MAX > continueProbability) {
							break;
						}
						pathThroughput /= continueProbability;
					}
					if (trueBounces == (*renderEnv->globals)[MaxDepth]) {
						break;
					}
					
					// transform sample to world space
					//currentSample.ray.direction = tangentToWorld(currentSample.ray.direction, Nn);
					currentSample.ray.origin = orig;
                    
                    // if we're a mirror, we don't increment the bounce
                    if (currentBrdf->brdfType == MirrorBrdf) {
                        --bounces;
                    }
                    trueBounces++;
					// find next vertex
					if (!renderEnv->accelerationStructure->intersect(&currentSample.ray, &isect)) {
						// exit.
						break;
					}
				}
			}
		}
		samples[nSample].col = Lo/(*renderEnv->globals)[LightSamples];
		samples[nSample].alpha = alpha;
	}
			
	pthread_exit(NULL);
}

void Renderer::outputStats(){
    LOG_INFO("*************************************");
	LOG_INFO("Outputting statistics.");
	
	// Time
	time_t renderEnd;
	time(&renderEnd);
	int totalTime = difftime(renderEnd, renderTime);
	LOG_INFO("Total render time: " 
             << floor(totalTime/60/60) << " h " 
             << floor((totalTime/60) % 60) << " min " 
             << totalTime % 60 << " sec.");
	
	LOG_INFO("Done outputting statistics.");
    LOG_INFO("*************************************\n");
}