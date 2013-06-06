//
//  matteMaterial.h
//  Aurora
//
//  Created by Espen Nordahl on 07/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_matteMaterial_h
#define Aurora_matteMaterial_h

#include "material.h"
#include "lambert.h"

namespace Aurora {
	class MatteMaterial : public Material {
	public:
		MatteMaterial( std::string name, int diffColorIndex, int numSamples, RenderEnvironment *renderEnv);
		BrdfState getBrdf( const Vector &Vn, const Vector &Nn, const ShadingGeometry &shdGeo ) ;

        void frameBegin();
        void frameEnd();
        
	private:
		Brdf * brdf;
        int colorIndex;
	};
}

#endif