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

OpenexrDisplay::OpenexrDisplay(int width, int height, std::string file, RenderEnvironment * renderEnv) :
Display::Display(width, height, "openExrDisplay:" + file, renderEnv){
	m_filename = file;
	m_pixel_buffer.resizeErase(width, m_height);
    m_multisample_buffer.resize(m_height);
	for (int i=0; i<m_height; i++) {
//        m_pixel_buffer[i].resize(width);
        m_multisample_buffer[i].resize(width);
		for (int j=0; j<width; j++) {
			Imf::Rgba &p = m_pixel_buffer[i][j];
			p.r = 0.f;
			p.g = 0.f;
			p.b = 0.f;
			p.a = 0.f;
            m_multisample_buffer[i][j] = 0;
		}
	}
    m_metadata["renderer"] = std::string("Aurora v") + VERSION;
}

#pragma mark -
#pragma mark pipeline
void OpenexrDisplay::frameBegin(){
    
}

void OpenexrDisplay::frameEnd(){
    
}



void OpenexrDisplay::setPixel(int _width, int _height, const Color &col, float alpha){
	Imf::Rgba &p = m_pixel_buffer[_height][_width];
	p.r = col.r;
	p.g = col.g;
	p.b = col.b;
	p.a = alpha;
}

void OpenexrDisplay::appendValue(int _width, int _height, const Color &col, float alpha){
    Color oldCol;
    float oldAlpha;
    getPixel(_width, _height, &oldCol, &oldAlpha);
    float sampleNum = ++m_multisample_buffer[_width][_height];
    float weight = 1.f/sampleNum;
    Color newCol = col;
    setPixel(_width, _height, oldCol*(1-weight) + newCol * weight,
                     oldAlpha*(1-weight) + alpha*weight);

}

void OpenexrDisplay::getPixel(int _width, int _height, Color *col, float *alpha){
	col->r = m_pixel_buffer[_height][_width].r;
	col->g = m_pixel_buffer[_height][_width].g;
	col->b = m_pixel_buffer[_height][_width].b;
	*alpha = (float)m_pixel_buffer[_height][_width].a;
}


void OpenexrDisplay::draw(int numLines){
    std::string tmpfile = m_filename + "_tmp";
    
    Imf::Header header(m_width,
            m_height,
            1, // aspect ratio
            Imath::V2f (0, 0), //screen window center
            1, // screen window width
            Imf::INCREASING_Y, // line order
            Imf::ZIP_COMPRESSION // compression
            );

    for (StringMap::iterator it = m_metadata.begin(); it != m_metadata.end(); ++it){
//        LOG_DEBUG("Inserting exr metatada: " << it->first << " : " << it->second);
        header.insert(it->first.c_str(), Imf::StringAttribute(it->second));
    }
    
	Imf::RgbaOutputFile file (  tmpfile.c_str(),
                                header,
                                Imf::WRITE_RGBA,
                                Imf::globalThreadCount());
	
        // init buffer
    file.setFrameBuffer (&m_pixel_buffer[0][0], 1, m_width);
    
        // write
	file.writePixels (numLines);
	int writeResult;
	writeResult = rename( tmpfile.c_str() , m_filename.c_str() );
    if (writeResult != 0) {
        LOG_ERROR("File rename for '" << tmpfile << "' to '" << m_filename << "' was unsuccessfull. Error: " << writeResult);
    }

}

void OpenexrDisplay::addMetadata(const std::string &key, const std::string &value){
    m_metadata[key] = value;
}