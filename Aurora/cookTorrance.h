//
//  cookTorrance.h
//  Aurora
//
//  Created by Espen Nordahl on 23/07/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_cookTorrance_h
#define Aurora_cookTorrance_h

#include "core.h"
#include "brdf.h"

namespace Aurora {
	class CookTorrance : public Brdf{
	public:
		CookTorrance(Color col, float exponent, int numSamples);
		
		Sample3D getSample(const Vector &Vn, const Vector &Nn, int depth, int thread);
		Color evalSampleTangent(const Vector &Ln, const Vector &Vn);
		Color evalSampleWorld(const Vector &Ln, const Vector &Vn, const Vector &Nn);
		float pdf(const Vector &Ln, const Vector &Vn, const Vector Nn) const;
        
	private:
        void generateSampleBuffer(int i, int t);
        std::vector<float> randomU[NUM_THREADS][3];
        std::vector<float> randomV[NUM_THREADS][3];
		Color color;
		float exponent;
        int numSamples;
	};
}

#endif
