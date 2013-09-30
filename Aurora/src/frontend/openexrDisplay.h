//
//  openexrDisplay.h
//  Aurora
//
//  Created by Espen Nordahl on 05/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_openexrDisplay_h
#define Aurora_openexrDisplay_h

#include "core.h"
#include "frontEndObject.h"

#include "ImfArray.h"
#include "ImfRgba.h"

#include <string.h>
#include <tbb/mutex.h>

namespace Aurora {
	class OpenexrDisplay : FrontEndObject{
	public:	
		OpenexrDisplay(int width, int height, std::string file, RenderEnvironment * renderEnv);
		
		void setPixel(int width, int height, const Color &col, float alpha);
        void appendValue(int width, int height, const Color &col, float alpha);
		void getPixel(int width, int height, Color *col, float *alpha);
		void draw(int numLines);		
        void addMetadata(const std::string &key, const std::string &value);
        
        void clear();
        void resize(int width, int height);
        
        int height() const;
        int width() const;
        
        void frameBegin();
        void frameEnd();
        
        const std::string filename() const;
        
        char *copy();

	private:
        int m_width;
        int m_height;
        Imf::Array2D<Imf::Rgba> m_pixel_buffer;
        bool m_hasBuffer;
        u_char *m_copied_buffer;
        std::vector<std::vector < int > > m_multisample_buffer;
        StringMap m_metadata;
		std::string m_filename;
        tbb::mutex m_copy_mutex;
	};
}


#endif
