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

#include "log.h"
#define lcontext LOG_Renderer

#include <iostream>
#include <pthread.h>
#include <math.h>

#include <tbb/parallel_for.h>

#include "jsonParser.h"

using namespace Aurora;

Renderer::Renderer( char *file ){
    filename = std::string(file);
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
    AdaptiveRndSampler2D *cameraSampler = new AdaptiveRndSampler2D();
    renderCam = Camera((*renderEnv.globals)[FieldOfView], (*renderEnv.globals)[ResolutionX], (*renderEnv.globals)[ResolutionY], (*renderEnv.globals)[PixelSamples], cameraSampler);
    
	if (!objects.size()) {
		LOG_ERROR("No objects found.");
	}
	
        // acceleration structure
    AccelerationStructure *accel;
    
    if ((*renderEnv.globals)[AccelStructure] != ACCEL_EMBREE) {
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
        accel = new KdTreeAccelerator(renderable, KD_INTERSECTCOST, KD_TRAVERSECOST, KD_EMPTYBONUS, (*renderEnv.globals)[KD_MaxLeaf], (*renderEnv.globals)[KD_MaxDepth]);
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
            Reference<Material> black = new ConstantMaterial("Not in use - lightsource", Color(0.f), &renderEnv);
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
    	
	LOG_INFO("Done building render environment.");
    LOG_INFO("*************************************\n");
}

class IntegrateParallel {
public:
    IntegrateParallel(int x, int y, int num_samples):
    m_x(x), m_y(y), m_num_samples(num_samples)
    {}
    
    void operator()(const tbb::blocked_range<size_t>& r) const{
        for(size_t i=r.begin(); i!=r.end(); ++i){
            
        }
    }
private:
    int m_x;
    int m_y;
    int m_num_samples;
};


struct threadargs{
	RenderEnvironment *renderEnv;
	Sample2D *sampleBuffer;
	int numSamples;
    int threadNum;
};

struct accumulationargs{
    OpenexrDisplay *display;
    int multisamples;
    int height;
    int width;
    time_t *beginDraw;
    time_t *endDraw;
    int *nRenderedSamples;
    float *renderProgress;
    std::vector<Sample2D> *renderedSamples;
    std::vector<std::vector < int > > *multisampleBuffer;
    bool finalSamples;
};

void *integrateThreaded( void *threadid );

void *accumulate( void * threaddata );

void Renderer::renderImageTBB(){
    tbb::parallel_for( tbb::blocked_range<size_t>(0,100), IntegrateParallel(1,2,3));
}

void Renderer::renderImage(){
    LOG_INFO("*************************************");
	LOG_INFO("Rendering image.");
	
	// start render timer
	time_t renderBegin;
	time(&renderBegin);
	
	// set up buffer and sampler
	int width = renderCam.getWidthSamples();
	int height = renderCam.getHeightSamples();
	int multisamples = renderCam.getPixelSamples();
    std::string fn = (*renderEnv.stringGlobals)["fileName"];
    if(fn == ""){
        LOG_ERROR("Render output filename is blank");
    }
    displayDriver = new OpenexrDisplay(width, height, fn, &renderEnv);

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

		
		// STORE SAMPLES
        accumulationargs accumArgs = {
            displayDriver,
            multisamples,
            height,
            width,
            &beginDraw,
            &endDraw,
            &nRenderedSamples,
            &renderProgress,
            &renderedSamples,
            &multisampleBuffer,
            finalSamples
        };
        accumulate((void *)&accumArgs);
        if (finalSamples) {
            break;
        }
	}
        // post frame
	
	LOG_INFO("Done rendering image.");
    LOG_INFO("*************************************\n");
}

void *accumulate( void * threaddata ){
    
    accumulationargs *data = (accumulationargs *)threaddata;
    
    for (int nSample = 0; nSample < data->renderedSamples->size(); nSample++){
            //Sample2D drawingSample = renderedSamples[nSample];
        Sample2D drawingSample = (*data->renderedSamples)[nSample];
        PixelRegion region = drawingSample.region;
        if (drawingSample.col.hasNaNs()) {
            drawingSample.col = Color(0.f);
        }
            // each sample is potentially stored in multiple pixels
        for (int i=region.xMin; i < region.xMax; i++) {
            for (int j=region.yMin; j < region.yMax; j++) {
                if ((*data->multisampleBuffer)[i][j] == 1) {
                    data->display->setPixel(i, j, drawingSample.col, drawingSample.alpha);
                }
                else{
                    Color oldCol;
                    float oldAlpha;
                    data->display->getPixel(i, j, &oldCol, &oldAlpha);
                    float sampleNum = (*data->multisampleBuffer)[i][j];
                    float weight = 1.f/sampleNum;
                    data->display->setPixel(i, j, oldCol*(1-weight) + drawingSample.col*weight,
                                     oldAlpha*(1-weight) + drawingSample.alpha*weight);
                }
            }
        }
        
        (*data->multisampleBuffer)[drawingSample.x][drawingSample.y]++;
        time(data->endDraw);
        if (difftime(*data->endDraw, *data->beginDraw) > DRAW_DELAY){
            *data->renderProgress = float(*data->nRenderedSamples)/(data->width * data->height * data->multisamples);
            LOG_INFO("Rendering : " << *data->renderProgress * 100.f << "% complete.");
            time(data->beginDraw);
            data->display->draw(data->height);
        }
        *data->nRenderedSamples += 1;
    }
}

inline float PowerHeuristic(int nf, float fPdf, int ng, float gPdf) {
     float f = nf * fPdf, g = ng * gPdf;
     return (f*f) / (f*f + g*g);
}

void *integrateThreaded( void *threadid ){
	
	struct threadargs *args;
	
	args = (struct threadargs*) threadid;
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
                        
                        
                            // For diffuse samples we don't need MIS
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
                        if (rayType == MirrorRay) {
                            for (int i=0; i < renderEnv->lights.size(); i++) {
                                Reference<Light> light = renderEnv->lights[i];
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
		}
        else{ // camera ray miss
            for (int i=0; i < renderEnv->lights.size(); i++) {
                Reference<Light> light = renderEnv->lights[i];
                if (light->lightType == type_envLight) {
                    Lo += light->eval(sample, sample.ray.direction) * (*renderEnv->globals)[LightSamples];
                    alpha = 1.;
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