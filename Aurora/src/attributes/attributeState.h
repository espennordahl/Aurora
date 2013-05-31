//
//  attributeState.h
//  Aurora
//
//  Created by Espen Nordahl on 01/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_attributeState_h
#define Aurora_attributeState_h

#include "core.h"
#include "material.h"

namespace Aurora {
	struct AttributeState{
	public:
		Color emmision;
		Material * material;
//        const Transform *cameraToWorld;
//        const Transform *worldToCamera;
//        const Transform *objectToWorld;
//        const Transform *worldToObject;
        const Transform *cameraToObject;
        const Transform *objectToCamera;
	};
}

#endif
