//
//  brdf.h
//  Aurora
//
//  Created by Espen Nordahl on 06/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_brdf_h
#define Aurora_brdf_h

#include "core.h"

namespace Aurora {
    
    enum BrdfType{
        MatteBrdf,
        SpecBrdf,
        MirrorBrdf
    };
    
	class Brdf : public ReferenceCounted {
	public:
		virtual Sample3D getSample(const Vector &Vn, const Vector &Nn, int depth, int thread) = 0;
		virtual Color evalSampleTangent(const Vector &Ln, const Vector &Vn) = 0;
		virtual Color evalSampleWorld(const Vector &Ln, const Vector &Vn, const Vector &Nn) = 0;
		virtual float pdf(const Vector &Ln, const Vector &Vn, const Vector Nn) const = 0;
        BrdfType brdfType;
        IntegrationDomain integrationDomain;
	};
}

#endif
