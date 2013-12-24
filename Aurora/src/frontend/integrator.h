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
#include "intersection.h"

namespace Aurora {
    
    class Integrator {
        
    public:
        struct IntegrationResult{
            Color color;
            float alpha;
            long raycount;
        };
        
        struct LocalGeometry{
            Point P;
            Vector Vn;
            Vector Nn;
        };

        Integrator(RenderEnvironment *renderEnv);
        
        IntegrationResult integrateCameraSample(Sample3D sample, int numSamples);
        
        IntegrationResult integrateDirect(const LocalGeometry &lg, Brdf *brdf, bxdfParameters *brdfParameters);
        
        IntegrationResult integrateDirectLight(const LocalGeometry &lg, Light *light, Brdf *brdf, bxdfParameters *brdfParameters);
        
        IntegrationResult integrateDirectMIS(const LocalGeometry &lg, Light *light, Brdf *brdf, bxdfParameters *brdfParameters);
        
    private:
        void updateLocalGeometry(const Sample3D &sample, LocalGeometry *lg, Intersection *isect, AttributeState *attrs);
        
        RenderEnvironment *m_render_environment;
    };
}

#endif /* defined(__Aurora__intergrator__) */
