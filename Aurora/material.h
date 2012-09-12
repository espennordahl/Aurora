//
//  material.h
//  Aurora
//
//  Created by Espen Nordahl on 01/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_material_h
#define Aurora_material_h

#include "core.h"
#include "brdf.h"

namespace Aurora {
	class Material : public ReferenceCounted {
	public:
		virtual Reference<Brdf> getBrdf( const Vector &Vn, const Vector &Nn ) const = 0;
	};
}

#endif
