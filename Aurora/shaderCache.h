//
//  shaderCache.h
//  Aurora
//
//  Created by Espen Nordahl on 28/09/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_shaderCache_h
#define Aurora_shaderCache_h

#include "core.h"

#include <map>
#include <string.h>
#include <vector>

namespace Aurora {
    
    struct cacheNode{
        int numEntries;
        std::vector<std::map<std::string, Color> > values;
    };
    
    class ShaderCache {
    public:
        ShaderCache();
        
        bool getShaderAttribute(int triangleIndex, Point barCoords, std::string brdfName, brdfParameters *params);
        void setShaderAttribute(int triangleIndex, std::string attrName, std::vector<brdfParameters> inputParams);

    private:
        std::map<int, cacheNode> cache;
    };
}

#endif
