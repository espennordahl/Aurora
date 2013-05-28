//
//  shadingGeometry.h
//  Aurora
//
//  Created by Espen Nordahl on 20/10/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_shadingGeometry_h
#define Aurora_shadingGeometry_h

namespace Aurora {
    
    struct ShadingGeometry{
        Point P;    // Intersection point
        Vector Ns;  // Shading normal
        Vector Ng;  // Geometric normal
        Vector dPdu;// Derivative of P in u dir
        Vector dPdv;// Dericative of P in v dir
        
            // TODO: implement these transforms
            //        const Transform *cameraToWorld;
            //        const Transform *worldToCamera;
            //        const Transform *objectToWorld;
            //        const Transform *worldToObject;
        const Transform *cameraToObject;
        const Transform *objectToCamera;
        uv st;              // Texture coordinates
        Point barCoords;    // Local position in triangle
        int triangleIndex; // Triangle id
    };
}

#endif
