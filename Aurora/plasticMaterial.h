//
//  plasticMaterial.h
//  Aurora
//
//  Created by Espen Nordahl on 14/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_plasticMaterial_h
#define Aurora_plasticMaterial_h

#include "material.h"

namespace Aurora {
	class PlasticMaterial : public Material {
	public:
		PlasticMaterial( const Color &col, float roughness, float reflectance, float _topLayerThickness, int numSamples);
		Reference<Brdf> getBrdf( const Vector &Vn, const Vector &Nn, int thread ) ;
		
	private:
		Reference<Brdf> diffBrdf;
		Reference<Brdf> specBrdf;
		float reflectance;
		float specGain;
	};
}

#endif