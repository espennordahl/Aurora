//
//  Sample.cpp
//  Aurora
//
//  Created by Espen Nordahl on 15/05/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "sample3D.h"

using namespace Aurora;

Sample3D::Sample3D(){
	ray = Ray();
	color = Color();
	pdf = 0;
}

Sample3D::Sample3D(const Ray &_Ray){
	ray = _Ray; color = Color(); pdf = 0;
}

Sample3D::Sample3D(const Ray &_Ray, float _pdf, const Color &_col):
ray(_Ray), pdf(_pdf), color(_col)
{
}