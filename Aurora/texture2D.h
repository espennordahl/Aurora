//
//  texture.h
//  Aurora
//
//  Created by Espen Nordahl on 13/08/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_texture_h
#define Aurora_texture_h

#include "core.h"

#include "ImfArray.h"
#include "ImfRgba.h"

#include <string.h>

namespace Aurora {
    class Texture2D {
    public:
        Texture2D(std::string filename);
        
        Color read(float s, float t, float filterSize);
        
    protected:
        Imf::Array2D<Imf::Rgba> pixelBuffer;
        int width;
        int height;
    };
}

#endif
