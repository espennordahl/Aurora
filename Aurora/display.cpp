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

Display::Display(int _width, int _height):
height(_height), width(_width)
{	
	// set up buffer size
	cPixels.reserve(width);
	aPixels.reserve(width);
	for (int i=0; i<width; i++) {
		//cPixels[i].reserve(height);
		//aPixels[i].reserve(height);
	}
	
	// initialize to black
	for (int i=0; i<width; i++) {
		for (int j=0; j<height; j++) {
			//		cPixels[i][j] = Color(0.f);
			//		aPixels[i][j] = 0.f; // TODO: Fix this
		}
	}
	
}

Display::Display(const std::vector<std::vector<Color> > &colors, 
					 const std::vector<std::vector<float> > &alpha):
cPixels(colors), aPixels(alpha)
{
	width = short(aPixels.size());
	height = short(aPixels[0].size());
}

Display::Display(const std::vector<std::vector<Color> > &colors):
cPixels(colors)
{
	width = short(aPixels.size());
	height = short(aPixels[0].size());
	
	aPixels.reserve(width);
	for (int i=0; i<width; i++) {
		aPixels[i].reserve(height);
	}
	
	// initialize alpha to white
	for (int i=0; i<width; i++) {
		for (int j=0; j<height; j++) {
			aPixels[i][j] = 1.f;
		}
	}
	
}

void Display::setPixel(int _width, int _height, const Color &color, float alpha){
	cPixels[_width][_height] = color;
	aPixels[_width][_height] = alpha;
}

// clear()
// Resets all pixel values to black
void Display::clear(){
	for (int i=0; i<width; i++) {
		for (int j=0; j<height; j++) {
			cPixels[i][j] = Color(0.f);
			aPixels[i][j] = 0.f;
		}
	}
}