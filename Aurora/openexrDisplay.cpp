//
//  openexrDisplay.cpp
//  Aurora
//
//  Created by Espen Nordahl on 05/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>

#include "ImfRgbaFile.h"
#include "ImfStringAttribute.h"
#include "ImfMatrixAttribute.h"
#include "ImfArray.h"
//#include "drawImage.h"

#include "OpenexrDisplay.h"

#include "log.h"
#define lcontext LOG_ExrDisplay

using namespace Aurora;

OpenexrDisplay::OpenexrDisplay(int _width, int _height, std::string file) : Display::Display(_width, _height){
	fileName = file;
	pixelBuffer.resizeErase(height, width);
    multisampleBuffer.resize(height);
	for (int i=0; i<height; i++) {
        multisampleBuffer[i].resize(width);
		for (int j=0; j<width; j++) {
			Imf::Rgba &p = pixelBuffer[i][j];
			p.r = 0.f;
			p.g = 0.f;
			p.b = 0.f;
			p.a = 0.f;
            multisampleBuffer[i][j] = 0;
		}
	}
}

void OpenexrDisplay::setPixel(int _width, int _height, const Color &col, float alpha){
	Imf::Rgba &p = pixelBuffer[_height][_width];
	p.r = col.r;
	p.g = col.g;
	p.b = col.b;
	p.a = alpha;
}

void OpenexrDisplay::appendValue(int _width, int _height, const Color &col, float alpha){
    Color oldCol;
    float oldAlpha;
    getPixel(_width, _height, &oldCol, &oldAlpha);
    float sampleNum = multisampleBuffer[_width][_height];
    float weight = 1.f/sampleNum;
    Color newCol = col;
    setPixel(_width, _height, oldCol*(1-weight) + newCol * weight,
                     oldAlpha*(1-weight) + alpha*weight);

}

void OpenexrDisplay::getPixel(int _width, int _height, Color *col, float *alpha){
	col->r = pixelBuffer[_height][_width].r;
	col->g = pixelBuffer[_height][_width].g;
	col->b = pixelBuffer[_height][_width].b;
	*alpha = (float)pixelBuffer[_height][_width].a;
}

void OpenexrDisplay::draw(int numLines){
	
	// open up file
	std::string tmpfile = fileName;
	tmpfile.append("_tmp");
	
	Imf::RgbaOutputFile file (tmpfile.c_str(), width, height, Imf::WRITE_RGBA,
							  1.f,
							  Imath::V2f (0, 0),
							  1.f,
							  Imf::INCREASING_Y,
							  Imf::NO_COMPRESSION,
							  Imf::globalThreadCount());
	
	// init buffer
    file.setFrameBuffer (&pixelBuffer[0][0], 1, width);
    
	// write
	file.writePixels (numLines);
	int result;
	result = rename( tmpfile.c_str() ,fileName.c_str() );
    if (result != 0) {
        LOG_ERROR("File rename for '" << tmpfile << "' to '" << fileName << "' was unsuccessfull. Error: " << result);
    }
}