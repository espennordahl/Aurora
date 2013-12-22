//
//  intergrator.h
//  Aurora
//
//  Created by Espen Nordahl on 16/12/13.
//  Copyright (c) 2013 Espen Nordahl. All rights reserved.
//

#ifndef __Aurora__intergrator__
#define __Aurora__intergrator__

#include <iostream>

#include "core.h"

#include "renderEnvironment.h"
#include "brdf.h"

namespace Aurora {
    
    class Integrator {
        
    public:
        struct IntegrationResult{
            Color color;
            float alpha;
            long raycount;
        };

        Integrator(RenderEnvironment *renderEnv);
        
        IntegrationResult integrateCameraSample(Sample3D sample, int numSamples);
        
        IntegrationResult integrateDirectLight(const Point &P, const Vector &Nn, const Vector &Vn, Light *light, Brdf *brdf, bxdfParameters *brdfParameters);
        
        IntegrationResult integrateDirectMIS(const Point &P, const Vector &Nn, const Vector &Vn, Light *light, Brdf *brdf, bxdfParameters *brdfParameters);
        
    private:
        RenderEnvironment *m_render_environment;
    };
}

#endif /* defined(__Aurora__intergrator__) */
