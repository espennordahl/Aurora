//
//  cookTorrance.h
//  Aurora
//
//  Created by Espen Nordahl on 23/07/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_cookTorrance_h
#define Aurora_cookTorrance_h

#include <tbb/atomic.h>
#include "halton_sampler.h"

#include "core.h"
#include "brdf.h"
#include "texture2D.h"
#include "openexrDisplay.h"

namespace Aurora {
    
    struct CookTorranceParameters : public bxdfParameters{
        CookTorranceParameters(const Color &specColor_, float roughness_, float reflectance_):
        color(specColor_), roughness(roughness_), reflectance(reflectance_){}
        
        Color color;
        float roughness;
        float reflectance;
    };

    
	class CookTorrance : public Brdf{
	public:
		CookTorrance(std::string name, RenderEnvironment *renderEnv);
		
		Sample3D getSample(const Vector &Vn, const Vector &Nn, bxdfParameters *parameters) const;
		Color evalSampleTangent(const Vector &Ln, const Vector &Vn, bxdfParameters *parameters) const;
		Color evalSampleWorld(const Vector &Ln, const Vector &Vn, const Vector &Nn, bxdfParameters *parameters) const;
		float pdf(const Vector &Ln, const Vector &Vn, const Vector Nn, bxdfParameters *parameters) const;
        
        void frameBegin();
        void frameEnd();

        
	private:
        void preCalcNormTable();
        float getNormWeight(float costheta, float roughness);
        
        Halton_sampler m_sampler;
        static tbb::atomic<int> m_halton_index;
	};
}

#endif
