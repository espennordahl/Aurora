//
//  blinn.h
//  Aurora
//
//  Created by Espen Nordahl on 12/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_blinn_h
#define Aurora_blinn_h

#include "core.h"
#include "brdf.h"

namespace Aurora {
	class Blinn : public Brdf{
	public:
		Blinn(Color col, float roughness, int numSamples);
		
		Sample3D getSample(const Vector &Vn, const Vector &Nn, int depth, int thread);
		Color evalSampleTangent(const Vector &Ln, const Vector &Vn, int thread);
		Color evalSampleWorld(const Vector &Ln, const Vector &Vn, const Vector &Nn, int thread);
		float pdf(const Vector &Ln, const Vector &Vn, const Vector Nn, int thread) const;
        void setParameters(brdfParameters *params, int thread){};

	private:
		Color color;
        void generateSampleBuffer(int i, int t);
        std::vector<float> randomU[NUM_THREADS][3];
        std::vector<float> randomV[NUM_THREADS][3];
        float roughness;
        int numSamples;
	};
}

#endif
