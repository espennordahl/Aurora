//
//  display.cpp
//  Aurora
//
//  Created by Espen Nordahl on 05/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>

#include "Display.h"


using namespace Aurora;

Display::Display(int width, int height, std::string name, RenderEnvironment *renderEnv):
FrontEndObject(name, renderEnv),
m_height(height), m_width(width)
{	
	// set up buffer size
	m_cPixels.reserve(width);
	m_aPixels.reserve(width);
	for (int i=0; i<width; i++) {
		//cPixels[i].reserve(height);
		//aPixels[i].reserve(height);
	}
	
	// initialize to black
	for (int i=0; i<width; i++) {
		for (int j=0; j<m_height; j++) {
			//		cPixels[i][j] = Color(0.f);
			//		aPixels[i][j] = 0.f; // TODO: Fix this
		}
	}
	
}

Display::Display(const std::vector<std::vector<Color> > &colors, 
					 const std::vector<std::vector<float> > &alpha, std::string name, RenderEnvironment *renderEnv):
FrontEndObject(name, renderEnv),
m_cPixels(colors), m_aPixels(alpha)
{
	m_width = short(m_aPixels.size());
	m_height = short(m_aPixels[0].size());
}

Display::Display(const std::vector<std::vector<Color> > &colors, std::string name, RenderEnvironment *renderEnv):
FrontEndObject(name, renderEnv),
m_cPixels(colors)
{
	m_width = short(m_aPixels.size());
	m_height = short(m_aPixels[0].size());
	
	m_aPixels.reserve(m_width);
	for (int i=0; i<m_width; i++) {
		m_aPixels[i].reserve(m_height);
	}
	
	// initialize alpha to white
	for (int i=0; i<m_width; i++) {
		for (int j=0; j<m_height; j++) {
			m_aPixels[i][j] = 1.f;
		}
	}
	
}

void Display::setPixel(int _width, int _height, const Color &color, float alpha){
	m_cPixels[_width][_height] = color;
	m_aPixels[_width][_height] = alpha;
}

// clear()
// Resets all pixel values to black
void Display::clear(){
	for (int i=0; i<m_width; i++) {
		for (int j=0; j<m_height; j++) {
			m_cPixels[i][j] = Color(0.f);
			m_aPixels[i][j] = 0.f;
		}
	}
}