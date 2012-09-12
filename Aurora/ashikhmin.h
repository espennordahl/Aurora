//
//  ashikhmin.h
//  Aurora
//
//  Created by Espen Nordahl on 12/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_ashikhmin_h
#define Aurora_ashikhmin_h

#include "core.h"
#include "brdf.h"

namespace Aurora {
	class Ashikhmin : public Brdf{
	public:
		Ashikhmin(Color col, float exp);
		
		Sample3D getSample(const Vector &Vn);
		Color evalSampleTangent(const Vector &Ln, const Vector &Vn);
		Color evalSampleWorld(const Vector &Ln, const Vector &Vn, const Vector &Nn);
		
	private:
		
		void sampleFirstQuadrant(float u1, float u2, float *phi, float *costheta) const;
		float calcPhi(float nu, float, float r1);
		Color color;
		float expX;
		float expY;
	};
}

#endif
