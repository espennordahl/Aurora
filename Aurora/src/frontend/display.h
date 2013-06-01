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
#include "frontEndObject.h"

namespace Aurora{
	
	class Display : FrontEndObject{
	protected:
		std::vector <std::vector<Color> >		m_cPixels;
		std::vector<std::vector<float> >		m_aPixels;
		int m_height;
		int m_width;
	public:
		Display(int width, int height, std::string name, RenderEnvironment *renderEnv);
		Display(const std::vector<std::vector<Color> > &colors, const std::vector<std::vector<float> > &alpha, std::string name, RenderEnvironment *renderEnv);
		Display(const std::vector<std::vector<Color> > &colors, std::string name, RenderEnvironment *renderEnv);
		
		virtual void setPixel(int width, int height, const Color &color, float alpha) = 0;
		
		virtual void clear() = 0;
        
        int height(){ return m_height; }
        
        virtual ~Display(){};
	};
}

#endif