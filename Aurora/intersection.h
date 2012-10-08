//
//  intersection.h
//  Aurora
//
//  Created by Espen Nordahl on 01/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_intersection_h
#define Aurora_intersection_h

#include "core.h"

namespace Aurora {
    
    struct ShadingGeometry{
        Point P;
        Vector Ns;
        Vector Ng;
        Vector dPdu;
        Vector dPdv;
            //        const Transform *cameraToWorld;
            //        const Transform *worldToCamera;
            //        const Transform *objectToWorld;
            //        const Transform *worldToObject;
        const Transform *cameraToObject;
        const Transform *objectToCamera;
        uv st;
        Point barCoords;
        int triangleIndex;
    };

	struct Intersection{
        ShadingGeometry shdGeo;
		Vector hitN;
		Point hitP;
		int attributesIndex;
	};
}

#endif
