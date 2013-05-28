//
//  sample2D.h
//  Aurora
//
//  Created by Espen Nordahl on 27/05/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Sample2D_h
#define Sample2D_h

#include "color.h"
#include "pixelRegion.h"

namespace Aurora {
	class Sample2D {
	public:
		PixelRegion region;
		int x;
		int y;
		Color col;
		float alpha;
		
		Sample2D();
		Sample2D(int _x, int _y);
	};
}


#endif
