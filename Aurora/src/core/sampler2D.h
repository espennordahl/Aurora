//
//  sampler2D.h
//  Aurora
//
//  Created by Espen Nordahl on 27/05/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Sampler2D_h
#define Sampler2D_h

#include "sample2D.h"
#include "pixelRegion.h"

#include <Vector>

namespace Aurora {
	class Sampler2D {
	protected:
		int currentSample;

	public:
		Sampler2D(){ };
		
		virtual void generateSamples( int _width, int _height, int multiSamples ) = 0;
		
		virtual bool nextSample( Sample2D *Sample ) = 0;
		
	};
}

#endif
