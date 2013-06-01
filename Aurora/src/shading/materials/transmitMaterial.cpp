////
////  transmitMaterial.cpp
////  Aurora
////
////  Created by Espen Nordahl on 3/09/12.
////  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
////
//
//#include <iostream>
//
//#include "transmitMaterial.h"
//
//using namespace Aurora;
//
//TransmitMaterial::TransmitMaterial( std::string name, Color col, float ior, RenderEnvironment *renderEnv ):
//Material(name, renderEnv)
//{
//    brdf = new SpecTransmit(name + ":SpecTransmit", col, ior, renderEnv);
//}
//
//void TransmitMaterial::frameBegin(){
//    
//}
//
//void TransmitMaterial::frameEnd(){
//    
//}
//
//Brdf * TransmitMaterial::getBrdf( const Vector &Vn, const Vector &Nn, const ShadingGeometry &shdGeo, bool mattePath, int thread ) {
//    return brdf;
//}
