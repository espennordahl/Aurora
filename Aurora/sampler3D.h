//
//  sampler3D.h
//  Aurora
//
//  Created by Espen Nordahl on 27/05/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Sampler3D_h
#define Sampler3D_h

#include "sample3D.h"

namespace Aurora {
	class Sampler3D  {
	protected:
		int currentSample;
	public:
		Sampler3D(){ };
		
		virtual void generateSamples( int _numSamples );
		
		virtual bool nextSample( Sample3D *Sample );
		
	};
}

#endif
