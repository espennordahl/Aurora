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
    m_brdfType = ConstantBrdf;
}

void Constant::frameBegin(){}
void Constant::frameEnd(){}

Sample3D Constant::getSample(const Vector &Vn, const Vector &Nn, bxdfParameters *parameters)const{
    return Sample3D();
}

Color Constant::evalSampleTangent(const Vector &Ln, const Vector &Vn, bxdfParameters *parameters)const{
    return col;
}

Color Constant::evalSampleWorld(const Vector &Ln, const Vector &Vn, const Vector &Nn, bxdfParameters *parameters)const{
    return col;
}

float Constant::pdf(const Vector &Ln, const Vector &Vn, const Vector Nn, bxdfParameters *parameters) const{
    return 0.f;
}
