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
    
    struct LambertParameters : public bxdfParameters{
        LambertParameters(const Color albedo_): albedo(albedo_){}
        
        Color albedo;
    };
    
	class Lambert : public Brdf{
	public:
		Lambert(std::string name, RenderEnvironment *renderEnv);
		
		inline Sample3D getSample(const Vector &Vn, const Vector &Nn, bxdfParameters *parameters) const;
		inline Color evalSampleTangent(const Vector &Ln, const Vector &Vn, bxdfParameters *parameters) const;
        inline Color evalSampleWorld(const Vector &Ln, const Vector &Vn, const Vector &Nn, bxdfParameters *parameters) const;
        inline float pdf(const Vector &Ln, const Vector &Vn, const Vector Nn, bxdfParameters *parameters) const;
		
        void frameBegin();
        void frameEnd();

	private:
        Halton_sampler m_sampler;
        
        static tbb::atomic<int> m_halton_index;
	};
    
    Sample3D Lambert::getSample(const Vector &Vn, const Vector &Nn, bxdfParameters *parameters) const{
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
        
        LambertParameters *params = static_cast<LambertParameters*>(parameters);
        
        Vector dir = SampleHemisphereUniform(r1, r2);
        return Sample3D(Ray(normalize(tangentToWorld(dir, Nn)), Point(0), RAY_BIAS, 1000000.f), M_PI_INV * 0.5, params->albedo * M_PI_INV);
    }
    
    Color Lambert::evalSampleTangent(const Vector &Ln, const Vector &Vn, bxdfParameters *parameters) const{
        LambertParameters *params = static_cast<LambertParameters*>(parameters);
        return params->albedo * M_PI_INV;
    }
    
    Color Lambert::evalSampleWorld(const Vector &Ln, const Vector &Vn, const Vector &Nn, bxdfParameters *parameters) const{
        float d = dot(Ln, Nn);
        if (d > 0.) {
            LambertParameters *params = static_cast<LambertParameters*>(parameters);
            return params->albedo * M_PI_INV ;
        }
        else{
            return Color(0);
        }
    }
    
    float Lambert::pdf(const Vector &Ln, const Vector &Vn, const Vector Nn, bxdfParameters *parameters) const{
        return M_PI_INV * 0.5;
    }
}

#endif
