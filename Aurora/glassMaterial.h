//
//  glassMaterial.h
//  Aurora
//
//  Created by Espen Nordahl on 3/09/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_glassMaterial_h
#define Aurora_glassMaterial_h



#include "material.h"

namespace Aurora {
	class GlassMaterial : public Material {
	public:
		GlassMaterial( std::string name, const Color &specCol, const Color &transCol, float reflectance, float ior );
		Reference<Brdf> getBrdf( const Vector &Vn, const Vector &Nn, const ShadingGeometry &shdGeo, int thread ) ;
        void frameBegin();
        void frameEnd();

	private:
		Reference<Brdf> refBrdf;
        Reference<Brdf> transmitBrdf;
        float reflectance;
	};
}

#endif
