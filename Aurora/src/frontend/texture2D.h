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
    public:
        Texture2D(std::string filename);
        
		Color read(float s, float t, float filterSize);
		float pdf(float s, float t, float filterSize);
        
		void initPdf();
		void sample(float r, float *x, float *y);
        
        int width(){ return m_width; };
        int height(){ return m_height; };
		
    protected:
		Imf::Array2D<Imf::Rgba> m_pixel_buffer;
        int m_width;
        int m_height;
        float m_pdf_normalization;
        std::vector<PdfSample> m_pdf_buffer;
        std::string m_texname;
    };
}

#endif
