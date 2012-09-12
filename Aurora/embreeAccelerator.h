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

#include "embree_accel.h"
#include "embree_intersector.h"

#include <vector>

namespace Aurora {
	class EmbreeAccelerator: public AccelerationStructure {
	protected:
        embree::Ref<embree::Intersector> intersector;
        EmbreeMesh mesh;
    public:
		EmbreeAccelerator(){ };
		EmbreeAccelerator(const EmbreeMesh &mesh);
		
		bool intersect( Ray *ray, Intersection *intersection) const;
		bool intersectBinary( Ray *ray ) const;
	};
}


#endif
