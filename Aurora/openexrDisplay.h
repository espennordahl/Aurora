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
#include "display.h"

#include "ImfArray.h"
#include "ImfRgba.h"

#include <string.h>


namespace Aurora {
	class OpenexrDisplay : public Display {
	public:	
		OpenexrDisplay(int _width, int _height, std::string file);
		
		void setPixel(int _width, int _height, const Color &col, float alpha);
        void appendValue(int _width, int _height, const Color &col, float alpha);
		void getPixel(int _width, int _height, Color *col, float *alpha);
		void draw(int numLines);		
		void clear(){}; // TODO: implement

	private:
        Imf::Array2D<Imf::Rgba> pixelBuffer;
        std::vector<std::vector < int > > multisampleBuffer;
		std::string fileName;
	};
}


#endif
