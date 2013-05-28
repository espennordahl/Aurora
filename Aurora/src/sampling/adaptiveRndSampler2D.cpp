//
//  adaptiveRndSampler2D.cpp
//  Aurora
//
//  Created by Espen Nordahl on 07/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>

#include "adaptiveRndSampler2D.h"

#include "log.h"
#define lcontext LOG_AdaptiveRndSampler


#include <math.h>



using namespace Aurora;

AdaptiveRndSampler2D::AdaptiveRndSampler2D(){
	
}

void AdaptiveRndSampler2D::generateSamples( int _width, int _height, int multiSamples ){
	regionNumber = 0;
	multisamples = multiSamples;
	width = _width;
	height = _height;
	remainingSamples = width * height;
	remainingBatches = multiSamples-1;
	
	LOG_DEBUG("Generating " << width << " * " << height << " * " <<  multisamples << " adaptiveRndsamples.");
	
	samples.resize(width);
	completedSamples.resize(width);
	for (int i=0; i<width; i++) {
		samples[i].resize(height);
		completedSamples[i].resize(height);
	}
	
	// generate samples
	for (int i=0; i < width; i++) {
		for (int j=0; j < height; j++) {
			samples[i][j] = Sample2D(i, j);
			completedSamples[i][j] = false;
		}
	}
	if (multisamples > 1){
		generateNewMulitSamples();
	}
	LOG_INFO("Done generating pixel samples. Total numer: " << width << " * " << height << " * " <<  multisamples << " pixel samples.");


}

void AdaptiveRndSampler2D::generateNewMulitSamples(){
	
	multiSampleBuffer.reserve(width * height);
	for (int i=0; i < width; i++) {
		for (int j=0; j < height; j++) {
			multiSampleBuffer.push_back(Sample2D(i, j));
		}
	}
	
	// shuffle
	for (int i=0; i < multiSampleBuffer.size(); i++) {
		int r = rand() % (int)multiSampleBuffer.size();
		Sample2D tmp = multiSampleBuffer[i];
		multiSampleBuffer[i] = multiSampleBuffer[r];
		multiSampleBuffer[r] = tmp;
	}

	remainingBatches--;
	
}
void AdaptiveRndSampler2D::initRegionBuffer(){
	++regionNumber;
	float aspectRatio = float(width)/height;
	int regionHeight = min(height, regionNumber * regionNumber);
	int regionWidth = ceil(regionHeight * aspectRatio);
	regionBuffer.clear();
//	LOG_DEBUG("Generating region buffer numer " << regionNumber << ", containing "
//			  << regionHeight*regionWidth << " regions.");
	for (int i=0; i < regionWidth; i++) {
		for (int j=0; j < regionHeight; j++) {
			PixelRegion region;
			region.xMin = i * floor(float(width)/regionWidth);
			region.xMax = region.xMin + floor(float(width)/regionWidth);
			if (i == regionWidth-1)
				region.xMax = width;
			region.yMin = j * floor(float(height)/regionHeight);
			region.yMax = region.yMin + floor(float(height)/regionHeight);
			if (j == regionHeight-1)
				region.yMax = height;
			regionBuffer.push_back(region);
			assert(region.xMin != region.xMax);
			assert(region.yMin != region.yMax);
		}
	}
	// shuffle
	for (int i=0; i < regionBuffer.size(); i++) {
		int r = rand() % (int)regionBuffer.size();
		PixelRegion tmp = regionBuffer[i];
		regionBuffer[i] = regionBuffer[r];
		regionBuffer[r] = tmp;
	}

}

bool AdaptiveRndSampler2D::nextSample( Sample2D *sample ){
	while (remainingSamples > 0) {
		// check if there are more sample regions in the current buffer
		if (regionBuffer.size() == 0){
			// if not, create a new sample region
			initRegionBuffer();
		}
		// pick first element
		PixelRegion region = regionBuffer.back();
		
		// first check if we already have a sample from this region
		
		bool skipSample = false;
		for (int i=region.xMin; i<region.xMax; i++) {
			for (int j=region.yMin; j<region.yMax; j++) {
				if (completedSamples[i][j] == true) {
					skipSample = true;
				}
			}
		}
		
		bool foundSample = false;
		if (skipSample == false) {
			// pick a random sample that fits
			int x = int(random() % (region.xMax - region.xMin) + region.xMin);
			int y = int(random() % (region.yMax - region.yMin) + region.yMin);
			// just to be absolutely certain
#ifdef DEBUG
			assert(completedSamples[x][y] == false);
#endif
			*sample = samples[x][y];
			sample->region = region;
			completedSamples[x][y] = true;
			remainingSamples--;
			foundSample = true;
#ifdef DEBUG
			assert(sample->x >= sample->region.xMin && sample->x < sample->region.xMax);
#endif
		}
		if (foundSample == true) {
			// we found a sample. Break out of the loop and return
			regionBuffer.pop_back();
			return true;
		}
		else{
			// If we reach the end of the loop, all the samples in the region
			// is already evaluated, so we keep going.
			regionBuffer.pop_back();
		}
	}
	if (multiSampleBuffer.size() != 0 || remainingBatches > 0) {
		if (!multiSampleBuffer.size()) {
			generateNewMulitSamples();
		}
		*sample = multiSampleBuffer.back();
		int x = sample->x;
		int y = sample->y;
		PixelRegion reg = PixelRegion(x, x+1, y, y+1);
		sample->region = reg;
		multiSampleBuffer.pop_back();
		return true;
	}
	return false;

}
