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
    struct PdfSample{
        float pdf;
        int x;
        int y;
        bool operator<(const PdfSample &s) const{
                return pdf < s.pdf;
        }
    };
    class Texture2D {
		Imf::Array2D<Imf::Rgba> pixelBuffer;
    public:
        Texture2D(std::string filename);
        
		Color read(float s, float t, float filterSize);
		float pdf(float s, float t, float filterSize);
        
		void initPdf();
		void sample(float r, float *x, float *y);
        
        int getWidth(){ return width; };
        int getHeight(){ return height; };
		
    protected:
        int width;
        int height;
        float pdfNormalization;
        std::vector<PdfSample> pdfBuffer;
    };
}

#endif
