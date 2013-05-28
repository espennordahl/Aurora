//
//  noise3D.cpp
//  Aurora
//
//  Created by Espen Nordahl on 25/09/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>

#include "noise3D.h"
#include "math_utils.h"

using namespace Aurora;

float Noise3D::eval(const Point &P){
    
        // compute noise cell coorindates and offsets
    int ix = floor(P.x);
    int iy = floor(P.y);
    int iz = floor(P.z);
    float dx = P.x - ix;
    float dy = P.y - iy;
    float dz = P.z - iz;
    
    
        // compute gradiant and weights
    ix &= (NOISE_PERM_SIZE-1);
    iy &= (NOISE_PERM_SIZE-1);
    iz &= (NOISE_PERM_SIZE-1);
    float w000 = Grad(ix,   iy,     iz,     dx,     dy,     dz);
    float w100 = Grad(ix+1, iy,     iz,     dx-1,   dy,     dz);
    float w010 = Grad(ix,   iy+1,   iz,     dx,     dy-1,   dz);
    float w110 = Grad(ix+1, iy+1,   iz,     dx-1,   dy-1,   dz);
    float w001 = Grad(ix,   iy,     iz+1,   dx,     dy,     dz-1);
    float w101 = Grad(ix+1, iy,     iz+1,   dx-1,   dy,     dz-1);
    float w011 = Grad(ix,   iy+1,   iz+1,   dx,     dy-1,   dz-1);
    float w111 = Grad(ix+1, iy+1,   iz+1,   dx-1,   dy-1,   dz-1);
    
        // compute trilinear interpolation of weights
    float wx = NoiseWeight(dx);
    float wy = NoiseWeight(dy);
    float wz = NoiseWeight(dz);
    
    float x00 = flerp(wx, w000, w100);
    float x10 = flerp(wx, w010, w110);
    float x01 = flerp(wx, w001, w101);
    float x11 = flerp(wx, w011, w111);
    float y0 = flerp(wy, x00, x10);
    float y1 = flerp(wy, x01, x11);
    
    return flerp(wz, y0, y1);
}