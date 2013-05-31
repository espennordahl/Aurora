//
//  renderEnvironment.h
//  Aurora
//
//  Created by Espen Nordahl on 01/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_renderEnvironment_h
#define Aurora_renderEnvironment_h

#include "core.h"
#include "options.h"

#include <vector>
#include <map>
#include <tr1/unordered_map>

    // forward declaration
namespace Aurora {
    class Light;
    class ShadingEngine;
    class Camera;
    class InfiniteAreaLight;
    class AttributeState;
    class AccelerationStructure;
}

namespace Aurora {
	struct RenderEnvironment{
		std::vector<Light* > lights;
		AccelerationStructure *accelerationStructure;
		AttributeState *attributeState;
        AuroraGlobals *globals;
        StringMap *stringGlobals;
		InfiniteAreaLight *envLight;
        ShadingEngine *shadingEngine;
		Camera *renderCam;
	};
}

#endif
