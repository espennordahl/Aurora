//
//  trPixelRegion.cpp
//  Aurora
//
//  Created by Espen Nordahl on 31/05/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "pixelRegion.h"

using namespace Aurora;

PixelRegion::PixelRegion(){
	xMin = 0; xMax = 0; yMin = 0; yMax = 0;
}

PixelRegion::PixelRegion(int _xMin, int _xMax, int _yMin, int _yMax):
xMin(_xMin), xMax(_xMax), yMin(_yMin), yMax(_yMax)
{
	
}

// isInside
// Checks if a Sample location is inside the region
bool PixelRegion::isInside(int x, int y){
	if (x >= xMin && x <= xMax && y >= yMin && y <= yMax)
		return true;
	else
		return false;
}