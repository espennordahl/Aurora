//
//  specTransmit.h
//  Aurora
//
//  Created by Espen Nordahl on 3/09/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_specTransmit_h
#define Aurora_specTransmit_h

#include "core.h"
#include "brdf.h"

namespace Aurora {
	class SpecTransmit : public Brdf{
	public:
		SpecTransmit(Color col, float ior);
		
		Sample3D getSample(const Vector &Vn, const Vector &Nn, int depth, int thread);
		Color evalSampleTangent(const Vector &Ln, const Vector &Vn);
		Color evalSampleWorld(const Vector &Ln, const Vector &Vn, const Vector &Nn);
		float pdf(const Vector &Ln, const Vector &Vn, const Vector Nn) const;
        
	private:
		Color color;
        float ior;
	};
}


#endif
