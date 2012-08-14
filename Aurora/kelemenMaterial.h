//
//  kelemenMaterial.h
//  Aurora
//
//  Created by Espen Nordahl on 23/07/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_kelemenMaterial_h
#define Aurora_kelemenMaterial_h

#include "material.h"

namespace Aurora {
	class KelemenMaterial : public Material {
	public:
		KelemenMaterial( Color diffCol, Color specCol, float exponent, float reflectance, int numSamples);
		Reference<Brdf> getBrdf( const Vector &Vn, const Vector &Nn ) const;
		
	private:
		Reference<Brdf> diffBrdf;
		Reference<Brdf> specBrdf;
		float reflectance;
        float exponent;
        float specGain;
        float diffGain;
	};
}

#endif
