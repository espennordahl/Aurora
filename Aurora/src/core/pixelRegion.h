//
//  trpixelRegion.h
//  Aurora
//
//  Created by Espen Nordahl on 31/05/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef trPixelRegion_h
#define trPixelRegion_h

namespace Aurora {
	class PixelRegion{
	public:
		int xMin;
		int xMax;
		int yMin;
		int yMax;
		
		PixelRegion();
		PixelRegion(int _xMin, int _xMax, int _yMin, int _yMax);
		
		bool isInside(int x, int y);
	};
}

#endif
