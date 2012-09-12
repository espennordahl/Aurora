//
//  accelerationStructure.h
//  Aurora
//
//  Created by Espen Nordahl on 01/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_dummyAccelerationStructure_h
#define Aurora_dummyAcelerationStructure_h

#include "core.h"
#include "accelerationStructure.h"

namespace Aurora {
	class DummyAccelerationStructure : public AccelerationStructure {
	public:
		DummyAccelerationStructure(const std::vector<RenderableTriangle> geometry);
		
		bool intersect( Ray *ray, Intersection *intersection) const;
		bool intersectBinary( Ray *ray ) const;
	
	protected:
		std::vector<RenderableTriangle> geometry;
		long numGeo;
	};
}

#endif
