//
//  adaptiveRndSampler2D.h
//  Aurora
//
//  Created by Espen Nordahl on 07/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_adaptiveRndSampler2D_h
#define Aurora_adaptiveRndSampler2D_h

#include "Sampler2D.h"

namespace Aurora {
	class AdaptiveRndSampler2D : public Sampler2D {
	protected:
		int width;
		int height;
		int multisamples;
		int regionNumber;
		int remainingSamples;
		int remainingBatches;
		std::vector<std::vector<Sample2D> > samples;
		std::vector<std::vector<bool> > completedSamples;
		std::vector<Sample2D> multiSampleBuffer;
		std::vector<PixelRegion> regionBuffer;
		
	public:
		AdaptiveRndSampler2D();
		void generateSamples( int _width, int _height, int multiSamples );
		
		bool nextSample( Sample2D *Sample );

	private:
		void initRegionBuffer();
		void generateNewMulitSamples();

	};
}


#endif
