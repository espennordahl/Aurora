//
//  display.h
//  Aurora
//
//  Created by Espen Nordahl on 05/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_display_h
#define Aurora_display_h

#include <vector>
#include "Color.h"

namespace Aurora{
	
	class Display {
	protected:
		std::vector <std::vector<Color> >		cPixels;
		std::vector<std::vector<float> >		aPixels;
		int height;
		int width;
	public:
		Display(int _width, int _height);
		Display(const std::vector<std::vector<Color> > &colors, const std::vector<std::vector<float> > &alpha);
		Display(const std::vector<std::vector<Color> > &colors);
		
		virtual void setPixel(int width, int height, const Color &color, float alpha) = 0;
		
		virtual void clear() = 0;
        virtual ~Display(){};
	};
}

#endif