//
//  transmitMaterial.h
//  Aurora
//
//  Created by Espen Nordahl on 3/09/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_transmitMaterial_h
#define Aurora_transmitMaterial_h


#include "material.h"
#include "specTransmit.h"

namespace Aurora {
	class TransmitMaterial : public Material {
	public:
		TransmitMaterial( Color col, float ior );
		Reference<Brdf> getBrdf( const Vector &Vn, const Vector &Nn ) const;
		
	private:
		Reference<Brdf> brdf;
	};
}

#endif
