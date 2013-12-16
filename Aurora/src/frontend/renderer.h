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
#include "integrator.h"

#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <map>

namespace Aurora {
    class Session;
}

namespace Aurora{

	class Renderer {
	public:
        Renderer(std::string file);
		void render();
        
	private:
        void parseSceneDescription();
		void buildRenderEnvironment();
		void renderImage();
		void outputStats();
		void postRenderCleanup();
        
		time_t m_renderTime;
		float m_renderProgress;
        std::string m_filename;
		Transform *m_cameraTransform;
        
        tbb::atomic<long> m_numrays;
        double m_rayspeed;

        Camera *m_renderCam;
		std::vector<std::tr1::shared_ptr<AuroraObject> > m_objects;
		std::vector<Light* > m_lights;
		InfiniteAreaLight *m_envLight;
		RenderEnvironment m_renderEnv;
		AttributeState *m_attrs;
		OpenexrDisplay *m_displayDriver;
        Integrator *m_integrator;
    };
}


#endif
