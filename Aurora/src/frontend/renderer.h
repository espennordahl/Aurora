//
//  renderer.h
//  Aurora
//
//  Created by Espen Nordahl on 30/06/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_renderer_h
#define Aurora_renderer_h

#include "camera.h"
#include "renderEnvironment.h"
#include "auroraObject.h"
#include "openexrDisplay.h"
#include "lights.h"
#include "options.h"

#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <map>

namespace Aurora {
    
	class Renderer {
	public:
        Renderer( char *file );
		void render();		
		Camera renderCam;
		std::vector<Reference<AuroraObject> > objects;
		std::vector<Reference<Light> > lights;
		InfiniteAreaLight *envLight;
		RenderEnvironment renderEnv;
		AttributeState *attrs;
		
	private:
		void parseSceneDescription();
		void buildRenderEnvironment();
		void renderImage();
		void outputStats();
		
		time_t renderTime;
		float renderProgress;
        std::string filename;
//        AuroraGlobals globals;
		Transform *cameraTransform;
		
		bool integrateSampleSINGLE(Sample3D *sample, float importance);
		bool integrateSampleMULTI(Sample3D *sample, float importance);
		OpenexrDisplay *displayDriver;
        
        pthread_t accumThread;
	};
}


#endif
