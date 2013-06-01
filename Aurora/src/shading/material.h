//
//  material.h
//  Aurora
//
//  Created by Espen Nordahl on 01/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_material_h
#define Aurora_material_h

#include "core.h"
#include "intersection.h"
#include "brdf.h"

    // forward declarations
namespace Aurora {
    class ShadingEngine;
}

namespace Aurora {
    struct LambertParameters{
        Color albedo;
    };

    
	class Material : public FrontEndObject {
	public:
        Material(std::string name, RenderEnvironment *renderEnv): FrontEndObject(name, renderEnv){};
            // Returns a brdf to run during light transport.
		virtual Brdf * getBrdf( const Vector &Vn, const Vector &Nn, const ShadingGeometry &shdGeo ) = 0;
        virtual LambertParameters getBrdfParameters(const ShadingGeometry &shdGeo, Brdf *brdf) = 0;
            // FrameBegin and FrameEnd are pipeline methods for doing
            // any kind of pre and post rendering work. This method
            // is ran on all front end objects recursively before and
            // after rendering
        virtual void frameBegin() = 0;
        virtual void frameEnd() = 0;
        
        ShadingEngine *m_shdEngine;
	};
}

#endif
