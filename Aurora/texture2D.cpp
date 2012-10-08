//
//  texture2D.cpp
//  Aurora
//
//  Created by Espen Nordahl on 13/08/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>

#include "texture2D.h"
#include "log.h"
#define lcontext LOG_Texture2D


#include "ImfRgbaFile.h"

#include <assert.h>

using namespace Aurora;

Texture2D::Texture2D(std::string filename){
    
    Imf::RgbaInputFile file (filename.c_str());
    Imath::Box2i dw = file.dataWindow();
    
    width = dw.max.x - dw.min.x + 1;
    height = dw.max.y - dw.min.y + 1;
	pixelBuffer.resizeErase(height, width);
    
	for (int i=0; i<height; i++) {
		for (int j=0; j<width; j++) {
			Imf::Rgba &p = pixelBuffer[i][j];
			p.r = 0.f;
			p.g = 0.f;
			p.b = 0.f;
			p.a = 0.f;
		}
	}
	
    file.setFrameBuffer (&pixelBuffer[0][0] - dw.min.x - dw.min.y * width, 1, width);
    file.readPixels (dw.min.y, dw.max.y);
}

Color Texture2D::read(float s, float t, float filterSize){
	int x = floor(s * width);
	int y = floor(t * height);
	if (y == height) {
		y -= 1;
	}
	if (x == width) {
		x -= 1;
	}
//	assert(x < width && x >= 0);
//	assert(y < height && y >= 0);
	if(x > width || x < 0){
//        LOG_WARNING("Texture call outside of 0-1 range");
        x = min(x,width);
        x = max(x,0);
    }
	if(y > height || y < 0){
//        LOG_WARNING("Texture call outside of 0-1 range");
        y = min(y,height);
        y = max(y,0);
    }
    if (filterSize == 0.) {
        Imf::Rgba &p = pixelBuffer[y][x];
        return Color(p.r, p.g, p.b);
    }
    else {
            // assume filter size is 1 for now
        float offsetX = (s*width) - x;
        float offsetY = (t*height) - y;
        float area1 = (1.-offsetX) * (1.-offsetY);
        float area2 = offsetX * (1.-offsetY);
        float area3 = (1-offsetX) * offsetY;
        float area4 = offsetX * offsetY;
        assert(softCompare(area1+area2+area3+area4, 1.));
        Color result = Color(0.f);
        Imf::Rgba &p = pixelBuffer[y][x];
        result += Color(p.r, p.g, p.b) * area1;
        p = pixelBuffer[y][x+1];
        result += Color(p.r, p.g, p.b) * area2;
        p = pixelBuffer[y+1][x];
        result += Color(p.r, p.g, p.b) * area3;
        p = pixelBuffer[y+1][x+1];
        result += Color(p.r, p.g, p.b) * area4;
        
        return result;
    }
}

float Texture2D::pdf(float s, float t, float filterSize){
	int x = floor(s * width);
	int y = floor(t * height);
	if (y == height) {
		y -= 1;
	}
	if (x == width) {
		x -= 1;
	}
        //	assert(x < width && x >= 0);
        //	assert(y < height && y >= 0);
	if(x > width || x < 0){
        LOG_WARNING("Texture call outside of 0-1 range");
        x = min(x,width);
        x = max(x,0);
    }
	if(y > height || y < 0){
        LOG_WARNING("Texture call outside of 0-1 range");
        y = min(y,height);
        y = max(y,0);
    }
    Imf::Rgba &p = pixelBuffer[y][x];
	
    return p.a/pdfNormalization;
}


void Texture2D::initPdf(){
	float lum = 0;
    pdfBuffer.clear();
	// we use the alpha for storing the pdf 
    // in addition to the buffer
	for (int i=0; i<height; i++) {
		for (int j=0; j<width; j++) {
			Imf::Rgba &p = pixelBuffer[i][j];
            PdfSample sample;
            sample.pdf = (p.r + p.g + p.b)/3.f;
            sample.x = j;
            sample.y = i;
            pdfBuffer.push_back(sample);
            p.a = sample.pdf;
			lum += sample.pdf;
		}
	}
	
//    std::sort(&pdfBuffer[0], &pdfBuffer[pdfBuffer.size()-1]);
    
    pdfNormalization = lum/(width * height);

//	for (int i=0; i<height; i++) {
//		for (int j=0; j<width; j++) {
//			Imf::Rgba &p = pixelBuffer[i][j];
//			p.a *= (1.f/lum);
//		}
//	}
        // normalize and distribute values along 0-1 for sampling
    float pdfAccum = 0;
    for (int i=0; i < pdfBuffer.size(); i++) {
//        pdfBuffer[i].pdf = pdfBuffer[i].pdf/(width * height);
        pdfBuffer[i].pdf += pdfAccum;
        pdfAccum = pdfBuffer[i].pdf;
        assert(pdfAccum <= lum);
    }    
}

void Texture2D::sample(float r, float *x, float *y){
	float offsetX = (float) rand()/RAND_MAX;
	float offsetY = (float) rand()/RAND_MAX;
//	r *= 0.9999;
	assert(r >= 0. && r <= 1.);
    
    r *= pdfNormalization * width * height;
        // clunky binary search. TODO: make more elegant
    if (r > pdfBuffer.back().pdf) {
        *x = (pdfBuffer.back().x + offsetX)/width;
        *y = (pdfBuffer.back().y + offsetY)/height;
        return;
    }
    if (r < pdfBuffer.front().pdf) {
        *x = (pdfBuffer.front().x + offsetX)/width;
        *y = (pdfBuffer.front().y + offsetY)/height;
        return;        
    }

    int depth = 1;
    int currentIndex;
    currentIndex = floor(pdfBuffer.size()/2);
    while (true) {
        ++depth;
        float tmp = pdfBuffer[currentIndex-1].pdf;
        if (r < tmp) {
            currentIndex -= floor(pdfBuffer.size()/(pow(2,depth)));
        }
        else {
            if (r > pdfBuffer[currentIndex].pdf) {
                currentIndex += floor(pdfBuffer.size()/(pow(2, depth)));
            }
            else {
                *x = (pdfBuffer[currentIndex].x + offsetX)/width;
                *y = (pdfBuffer[currentIndex].y + offsetY)/height;
                return;
            }
        }
    } 
    assert(true == false);
}