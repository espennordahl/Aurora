//
//  frontEndObject.h
//  Aurora
//
//  Created by Espen Nordahl on 7/10/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_frontEndObject_h
#define Aurora_frontEndObject_h

#include "renderEnvironment.h"
#include <string.h>

namespace Aurora {
    
    class FrontEndObject{
    public:
        FrontEndObject(std::string objName, RenderEnvironment *_renderEnv):
        renderEnv(_renderEnv),
        name(objName)
        {};
        virtual ~FrontEndObject(){};

        virtual void frameBegin() = 0;
        virtual void frameEnd() = 0;
        
        std::string name;
        RenderEnvironment *renderEnv;
    };
}

#endif
