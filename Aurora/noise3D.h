//
//  noise3D.h
//  Aurora
//
//  Created by Espen Nordahl on 25/09/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_noise3D_h
#define Aurora_noise3D_h

#include "core.h"

namespace Aurora {
    class Noise3D {
    public:
        Noise3D(){};
        
        float eval(const Point &P);
    };
}

#endif
