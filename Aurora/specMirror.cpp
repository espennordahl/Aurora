//
//  specMirror.cpp
//  Aurora
//
//  Created by Espen Nordahl on 3/09/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>

#include "specMirror.h"

using namespace Aurora;

SpecMirror::SpecMirror(Color col):
color(col){
    brdfType = MirrorBrdf;
}

Sample3D SpecMirror::getSample(const Vector &Vn, const Vector &Nn, int depth, int thread){
    Vector Vt = worldToTangent(Vn, Nn);
    Vector R = Vector(-Vt.x, -Vt.y, Vt.z);
    R = tangentToWorld(R, Nn);
    return Sample3D(Ray(normalize(R), Point(0.), RAY_BIAS, 100000000.f), 1.f, color);
}

Color SpecMirror::evalSampleTangent(const Vector &Ln, const Vector &Vn){
    return Color(0.f);
}

Color SpecMirror::evalSampleWorld(const Vector &Ln, const Vector &Vn, const Vector &Nn){
    return Color(0.f);
}

float SpecMirror::pdf(const Vector &Ln, const Vector &Vn, const Vector Nn) const{
    return 0.f;
}
