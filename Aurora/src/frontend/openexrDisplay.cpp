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
FrontEndObject("Display:" + file, renderEnv),
m_width(width),
m_height(height){
	m_filename = file;
    resize(width, height);
    m_metadata["renderer"] = std::string("Aurora v") + VERSION;
}

#pragma mark -
#pragma mark pipeline
void OpenexrDisplay::frameBegin(){
    
}

void OpenexrDisplay::frameEnd(){
    
}

int OpenexrDisplay::width() const{
    return m_width;
}

int OpenexrDisplay::height() const {
    return m_height;
}

void OpenexrDisplay::setPixel(int x, int y, const Color &col, float alpha){
	Imf::Rgba &p = m_pixel_buffer[y][x];
	p.r = col.r;
	p.g = col.g;
	p.b = col.b;
	p.a = alpha;
}

void OpenexrDisplay::appendValue(int x, int y, const Color &col, float alpha){
    Color oldCol;
    float oldAlpha;
    getPixel(x, y, &oldCol, &oldAlpha);
    m_multisample_buffer[y][x] += 1;
    float sampleNum = m_multisample_buffer[y][x];
    float weight = 1.f/sampleNum;
    Color newCol = col;
    setPixel(x, y, oldCol*(1-weight) + newCol * weight,
                     oldAlpha*(1-weight) + alpha*weight);

}

void OpenexrDisplay::getPixel(int x, int y, Color *col, float *alpha){
    Imf::Rgba &p = m_pixel_buffer[y][x];
	col->r = p.r;
	col->g = p.g;
	col->b = p.b;
	*alpha = (float)p.a;
}

void OpenexrDisplay::clear(){
    for (int i=0; i<m_height; i++) {
		for (int j=0; j<m_width; j++) {
			Imf::Rgba &p = m_pixel_buffer[i][j];
			p.r = 0.f;
			p.g = 0.f;
			p.b = 0.f;
			p.a = 0.f;
            m_multisample_buffer[i][j] = 0;
		}
	}
}

void OpenexrDisplay::resize(int width, int height){
    m_copy_mutex.lock();
    
    m_pixel_buffer.resizeErase(height, width);
    
    for (int i=0; i<m_multisample_buffer.size(); ++i) {
        m_multisample_buffer[i].clear();
    }
    m_multisample_buffer.clear();

    m_multisample_buffer.resize(height);
	for (int i=0; i<height; i++) {
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

    m_width = width;
    m_height = height;
    
    m_copy_mutex.unlock();
}


void OpenexrDisplay::draw(int numLines){
    std::string tmpfile = m_filename + "_tmp";
    
    Imf::Header header( m_width,
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

const std::string OpenexrDisplay::filename() const{
    return m_filename;
}

char *OpenexrDisplay::copy(){
    m_copy_mutex.lock();
    m_copied_buffer.resizeErase(m_height, m_width);
	for (int i=0; i<m_height; i++) {
        m_multisample_buffer[i].resize(m_width);
		for (int j=0; j<m_width; j++) {
            m_copied_buffer[i][j] = m_pixel_buffer[i][j];
        }
    }
    m_copy_mutex.unlock();
    return (char*)&m_copied_buffer[0][0];
}