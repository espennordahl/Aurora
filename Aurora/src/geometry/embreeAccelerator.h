//
//  embreeAccelerator.h
//  Aurora
//
//  Created by Espen Nordahl on 4/09/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_embreeAccelerator_h
#define Aurora_embreeAccelerator_h

#include "core.h"
#include "renderableTriangle.h"
#include "accelerationStructure.h"
#include "embreeMesh.h"

#include "accel.h"
#include "intersector.h"

#include <vector>

namespace Aurora {
	class EmbreeAccelerator: public AccelerationStructure {
    public:
		EmbreeAccelerator(){ };
        ~EmbreeAccelerator();
		EmbreeAccelerator(const EmbreeMesh &mesh, AttributeState *attrs);
		
		bool intersect( Ray *ray, Intersection *intersection) const;
		bool intersectBinary( Ray *ray ) const;
        void getShadingTriangles( int triangleIndex, ShadingGeometry *shdGeo );
        
    private:
        embree::Ref<embree::Intersector> m_intersector;
        EmbreeMesh m_mesh;

	};
}


#endif
