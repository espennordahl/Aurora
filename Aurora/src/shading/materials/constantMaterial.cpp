//
//  constantMaterial.cpp
//  Aurora
//
//  Created by Espen Nordahl on 26/12/12.
//
//

#include "constantMaterial.h"

using namespace Aurora;

ConstantMaterial::ConstantMaterial( std::string name, Color col, RenderEnvironment *renderEnv):
Material(name, renderEnv)
{
    brdf = new Constant(name, col, renderEnv);
}

BrdfState ConstantMaterial::getBrdf( const Vector &Vn, const Vector &Nn, const ShadingGeometry &shdGeo, bool mattePath){
    BrdfState state;
    state.brdf = brdf;
    state.parameters = new bxdfParameters();
    return state;
}

void ConstantMaterial::frameBegin(){};
void ConstantMaterial::frameEnd(){};
