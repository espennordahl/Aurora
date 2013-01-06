//
//  core.h
//  Aurora
//
//  Created by Espen Nordahl on 30/06/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef core_h
#define core_h


// 3D

#include "point.h"
#include "vector.h"
#include "ray.h"
#include "sample3D.h"
#include "sampler3D.h"
#include "transform.h"
#include "bbox.h"
#include "matrix4x4.h"
#include "uv.h"
#include "shadingGeometry.h"

// 2D

#include "color.h"
#include "pixelRegion.h"
#include "sample2D.h"
#include "sampler2D.h"

// Memory

#include "pbrtParalallel.h"
#include "reference.h"
#include "referenceCounted.h"

// Util
#include "stringUtils.h"
#include "typedefs.h"
#include "constants.h"
#include "shaderAttribute.h"

#endif
