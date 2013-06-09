//
//  uv.h
//  Aurora
//
//  Created by Espen Nordahl on 22/09/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_uv_h
#define Aurora_uv_h

namespace Aurora {
    struct uv{
        uv():u(0.f),v(0.f){}
        uv(float s, float t):u(s), v(t){}
        float u;
        float v;
    };
}

#endif
