//
//  constant.cpp
//  Aurora
//
//  Created by Espen Nordahl on 26/12/12.
//
//

#include "constant.h"

using namespace Aurora;

Constant::Constant(std::string name, Color _col, RenderEnvironment *renderEnv):
Brdf(name, renderEnv),
col(_col)
{
}

void Constant::frameBegin(){}
void Constant::frameEnd(){}

Sample3D Constant::getSample(const Vector &Vn, const Vector &Nn, int depth, int thread){
    return Sample3D();
}

Color Constant::evalSampleTangent(const Vector &Ln, const Vector &Vn, int thread){
    return col;
}

Color Constant::evalSampleWorld(const Vector &Ln, const Vector &Vn, const Vector &Nn, int thread){
    return col;
}

float Constant::pdf(const Vector &Ln, const Vector &Vn, const Vector Nn, int thread) const{
    return 0.f;
}
