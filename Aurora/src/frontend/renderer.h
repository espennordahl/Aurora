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
    class Session;
}

namespace Aurora{

	class Renderer {
	public:
        Renderer(std::string file);
        void parseSceneDescription();
		void render();
		void setDelegate(Session *delegate);

        void stop();
        void setResolution(int width, int height);
        bool isRunning();
        
		Camera *renderCam;
		std::vector<std::tr1::shared_ptr<AuroraObject> > objects;
		std::vector<Light* > lights;
		InfiniteAreaLight *envLight;
		RenderEnvironment renderEnv;
		AttributeState *attrs;
		OpenexrDisplay *displayDriver;
    
	private:
		void buildRenderEnvironment();
		void renderImage();
		void renderImageTBB();
		void outputStats();
		void postRenderCleanup();
        
		time_t renderTime;
		float renderProgress;
        std::string filename;
		Transform *cameraTransform;

        bool m_stopped;
		bool m_rendering;
        
        tbb::atomic<long> m_numrays;
        double m_rayspeed;
        Session *m_delegate;
        
        int m_widthOverride;
        int m_heightOverride;
	};
}


#endif
