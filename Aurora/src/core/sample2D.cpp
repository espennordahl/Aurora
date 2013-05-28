//
//  Sample2D.cpp
//  Aurora
//
//  Created by Espen Nordahl on 27/05/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "sample2D.h"

using namespace Aurora;

Sample2D::Sample2D(){
	x = 0; y = 0; col = Color(0.f); alpha = 0.f;
}

Sample2D::Sample2D(int _x, int _y):
x(_x), y(_y){
	col = Color(0.f); alpha = 0.f;
}
