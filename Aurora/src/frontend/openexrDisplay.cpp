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
m_height(height),
m_hasBuffer(false)
{
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
    m_copy_mutex.lock();

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
    m_copy_mutex.unlock();
}

void OpenexrDisplay::resize(int width, int height){
    m_copy_mutex.lock();
    
    if(m_hasBuffer){
        delete[] m_copied_buffer;
        m_hasBuffer = false;
    }
    
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
    
    if(!m_hasBuffer){
        m_copied_buffer = new u_char[m_height*m_width*4];
        m_hasBuffer = true;
    }
    u_char *channel = m_copied_buffer;
	
    for (int i=0; i<m_height; i++) {
		for (int j=0; j<m_width; j++) {
			Imf::Rgba &p = m_pixel_buffer[i][j];
            *channel = 255 * pow(MIN((float)p.r, 1.), 0.45454545);
            ++channel;
            *channel = 255 * pow(MIN((float)p.g, 1.), 0.45454545);
            ++channel;
            *channel = 255 * pow(MIN((float)p.b, 1.), 0.45454545);
            ++channel;
            *channel = 255 * MIN((float)p.a, 1.);
            ++channel;
		}
	}
    
    m_copy_mutex.unlock();

    return (char*)m_copied_buffer;
}

char *OpenexrDisplay::proxy(int level){
    m_copy_mutex.lock();
    
    if(!m_hasBuffer){
        m_copied_buffer = new u_char[m_height*m_width*4];
        m_hasBuffer = true;
    }
    u_char *channel = m_copied_buffer;
	
    for (int i=0; i<m_height; i++) {
		for (int j=0; j<m_width; j++) {
            int y = i - i % level;
            int x = j - j % level;
            float r = 0;
            float g = 0;
            float b = 0;
            float a = 0;
            float invLevelSquared = 1. / (level * level);
            for (int lx=0; lx < level; ++lx) {
                for (int ly=0; ly < level; ++ly) {
                    Imf::Rgba &p = m_pixel_buffer[MIN(y + ly, m_height-1)][MIN(x + lx, m_width-1)];
                    r += p.r * invLevelSquared;
                    g += p.g * invLevelSquared;
                    b += p.b * invLevelSquared;
                    a += p.a * invLevelSquared;
                }
            }
            *channel = 255 * pow(MIN(r, 1.), 0.45454545);
            ++channel;
            *channel = 255 * pow(MIN(g, 1.), 0.45454545);
            ++channel;
            *channel = 255 * pow(MIN(b, 1.), 0.45454545);
            ++channel;
            *channel = 255 * MIN(a, 1.);
            ++channel;
		}
	}
    
    m_copy_mutex.unlock();
    
    return (char*)m_copied_buffer;
}