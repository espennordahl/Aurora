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
		
		inline Sample3D getSample(const Vector &Vn, const Vector &Nn, const LambertParameters &parameters) const;
		inline Color evalSampleTangent(const Vector &Ln, const Vector &Vn, const LambertParameters &parameters) const;
        inline Color evalSampleWorld(const Vector &Ln, const Vector &Vn, const Vector &Nn, const LambertParameters &parameters) const;
        inline float pdf(const Vector &Ln, const Vector &Vn, const Vector Nn) const;
		
        void frameBegin();
        void frameEnd();

	private:
        Halton_sampler m_sampler;
        
        static tbb::atomic<int> m_halton_index;
	};
    
    Sample3D Lambert::getSample(const Vector &Vn, const Vector &Nn, const LambertParameters &parameters) const{
#ifdef USE_HALTON
        const int i = m_halton_index.fetch_and_increment();
        float r1 = m_sampler.sample(SURFACE_DOMAIN_X, i);
        float r2 = m_sampler.sample(SURFACE_DOMAIN_Y, i);
#else
        float r1 = (float) rand()/RAND_MAX;
        float r2 = (float) rand()/RAND_MAX;
#endif
        assert(r1 >= 0. && r1 <= 1.f);
        assert(r2 >= 0. && r2 <= 1.f);
        
        Vector dir = SampleHemisphereUniform(r1, r2);
        return Sample3D(Ray(normalize(tangentToWorld(dir, Nn)), Point(0), RAY_BIAS, 1000000.f), M_PI_INV * 0.5, parameters.albedo * M_PI_INV);
    }
    
    Color Lambert::evalSampleTangent(const Vector &Ln, const Vector &Vn, const LambertParameters &parameters) const{
        return parameters.albedo * M_PI_INV;
    }
    
    Color Lambert::evalSampleWorld(const Vector &Ln, const Vector &Vn, const Vector &Nn, const LambertParameters &parameters) const{
        float d = dot(Ln, Nn);
        if (d > 0.) {
            return parameters.albedo * M_PI_INV ;
        }
        else{
            return Color(0);
        }
    }
    
    float Lambert::pdf(const Vector &Ln, const Vector &Vn, const Vector Nn) const{
        return M_PI_INV * 0.5;
    }
}

#endif
