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

namespace Aurora {
    
    class Integrator {
        
    public:
        struct IntegrationResult{
            Color color;
            float alpha;
            long raycount;
        };

        Integrator(RenderEnvironment *renderEnv);
        
        IntegrationResult integrateSample(Sample3D sample, int numSamples);
        
    private:
        RenderEnvironment *m_render_environment;
    };
}

#endif /* defined(__Aurora__intergrator__) */
