//
//  lambert.h
//  Aurora
//
//  Created by Espen Nordahl on 06/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_lambert_h
#define Aurora_lambert_h

#include "core.h"
#include "brdf.h"

namespace Aurora {
	class Lambert : public Brdf{
	public:
		Lambert(Color col, int numSamples);
		
		Sample3D getSample(const Vector &Vn, const Vector &Nn, int depth, int thread);
		Color evalSampleTangent(const Vector &Ln, const Vector &Vn);
		Color evalSampleWorld(const Vector &Ln, const Vector &Vn, const Vector &Nn);
		
		float pdf(const Vector &Ln, const Vector &Vn, const Vector Nn) const;


	private:
		Color color;
        std::vector<float> randomU[NUM_THREADS][3];
        std::vector<float> randomV[NUM_THREADS][3];
        int numSamples;
        void generateSampleBuffer(int i, int t);
	};
}

#endif
