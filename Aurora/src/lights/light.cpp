//
//  light.cpp
//  Aurora
//
//  Created by Espen Nordahl on 06/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//


#include "light.h"

#include <iostream>
#include <assert.h>

#include "log.h"
#define lcontext LOG_Lights


using namespace Aurora;

Light::Light( Transform *o2c, Transform *c2o, Transform *o2w, Transform *w2o, Transform *c2w, Transform *w2c, float _exposure, Color _color, std::string name, RenderEnvironment *renderEnv) :
    FrontEndObject(name, renderEnv),
	objectToCamera(o2c), 
	cameraToObject(c2o),
	worldToObject(w2o),
	objectToWorld(o2w),
	cameraToWorld(c2w),
	worldToCamera(w2c),
	exposure(_exposure), 
	color(_color)
{
	
}