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
#include "material.h"

namespace Aurora {
	struct Intersection{
		Vector hitN;
		Point hitP;
		int attributesIndex;
	};
}

#endif
