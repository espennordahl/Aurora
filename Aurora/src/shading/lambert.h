//
//  lambert.h
//  Aurora
//
//  Created by Espen Nordahl on 06/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_lambert_h
#define Aurora_lambert_h

#include <tbb/atomic.h>

#include "core.h"
#include "brdf.h"
#include "halton_sampler.h"

namespace Aurora {
    
	class Lambert : public Brdf{
	public:
		Lambert(std::string name, RenderEnvironment *renderEnv);
		
		Sample3D getSample(const Vector &Vn, const Vector &Nn, const LambertParameters &parameters) const;
		Color evalSampleTangent(const Vector &Ln, const Vector &Vn, const LambertParameters &parameters) const;
		Color evalSampleWorld(const Vector &Ln, const Vector &Vn, const Vector &Nn, const LambertParameters &parameters) const;
		
        void frameBegin();
        void frameEnd();
        float pdf(const Vector &Ln, const Vector &Vn, const Vector Nn) const;

	private:
        Halton_sampler m_sampler;
        
        static tbb::atomic<int> m_halton_index;
	};
}

#endif
