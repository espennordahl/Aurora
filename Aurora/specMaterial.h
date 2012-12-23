//
//  specMaterial.h
//  Aurora
//
//  Created by Espen Nordahl on 12/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_specMaterial_h
#define Aurora_specMaterial_h

#include "material.h"
#include "ashikhmin.h"

namespace Aurora {
	class SpecMaterial : public Material {
	public:
		SpecMaterial( const Color &col, float roughness, int numSamples);
		Reference<Brdf> getBrdf( const Vector &Vn, const Vector &Nn, int thread ) ;
		
        
        
	private:
		Color color;
		Reference<Brdf> brdf;
	};
}

#endif
