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
#include "frontEndObject.h"

namespace Aurora {
    enum BrdfType{
        MatteBrdf,
        SpecBrdf,
        MirrorBrdf
    };
    
	class Brdf : public ReferenceCounted, public FrontEndObject {
	public:
        Brdf(std::string objName, RenderEnvironment *renderEnv):FrontEndObject(objName, renderEnv){};
        
		virtual Sample3D getSample(const Vector &Vn, const Vector &Nn, int depth, int thread) = 0;
		virtual Color evalSampleTangent(const Vector &Ln, const Vector &Vn, int thread) = 0;
		virtual Color evalSampleWorld(const Vector &Ln, const Vector &Vn, const Vector &Nn, int thread) = 0;
		virtual float pdf(const Vector &Ln, const Vector &Vn, const Vector Nn, int thread) const = 0;
        virtual void setParameters(void *params, int thread) = 0;
        virtual void initRoughness(bool mattePath, int thread) = 0;
        virtual ~Brdf(){};
        BrdfType brdfType;
        IntegrationDomain integrationDomain;
	};
}

#endif
