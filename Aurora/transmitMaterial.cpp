//
//  transmitMaterial.cpp
//  Aurora
//
//  Created by Espen Nordahl on 3/09/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>

#include "transmitMaterial.h"

using namespace Aurora;

TransmitMaterial::TransmitMaterial( Color col, float ior ){
    brdf = new SpecTransmit(col, ior);
}

Reference<Brdf> TransmitMaterial::getBrdf( const Vector &Vn, const Vector &Nn ) const{
    return brdf;
}
