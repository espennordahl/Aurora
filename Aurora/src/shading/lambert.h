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
    
    struct lambertParameters{
        Color albedo;
    };

    
	class Lambert : public Brdf{
	public:
		Lambert(std::string name, Color col, int numSamples, RenderEnvironment *renderEnv);
		
		Sample3D getSample(const Vector &Vn, const Vector &Nn, int depth, int thread);
		Color evalSampleTangent(const Vector &Ln, const Vector &Vn, int thread);
		Color evalSampleWorld(const Vector &Ln, const Vector &Vn, const Vector &Nn, int thread);
		
        void frameBegin();
        void frameEnd();
        void initRoughness(bool mattePath, int thread){};
        float pdf(const Vector &Ln, const Vector &Vn, const Vector Nn, int thread) const;
        void setParameters(void *params, int thread);

	private:
        Color color[NUM_THREADS];
        std::vector<float> randomU[NUM_THREADS][3];
        std::vector<float> randomV[NUM_THREADS][3];
        int numSamples;
        void generateSampleBuffer(int i, int t);
	};
}

#endif
