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
#include "accelerationStructure.h"
#include "attributeState.h"
#include "light.h"
#include "infiniteAreaLight.h"
#include "options.h"

#include <vector>
#include <map>

namespace Aurora {
	struct RenderEnvironment{
		std::vector<Reference<Light> > lights;
		AccelerationStructure *accelerationStructure;
		AttributeState *attributeState;
        std::map<Option, double> *globals;
		InfiniteAreaLight *envLight;
		Camera *renderCam;
	};
}

#endif
