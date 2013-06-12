//
//  specMirror.h
//  Aurora
//
//  Created by Espen Nordahl on 3/09/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_specMirror_h
#define Aurora_specMirror_h

#include "core.h"
#include "brdf.h"

namespace Aurora {
	class SpecMirror : public Brdf{
	public:
		SpecMirror(std::string name, Color col, RenderEnvironment *renderEnv);
		
		Sample3D getSample(const Vector &Vn, const Vector &Nn, bxdfParameters *parameters) const;
		Color evalSampleTangent(const Vector &Ln, const Vector &Vn, bxdfParameters *parameters) const ;
		Color evalSampleWorld(const Vector &Ln, const Vector &Vn, const Vector &Nn, bxdfParameters *parameters) const;
		float pdf(const Vector &Ln, const Vector &Vn, const Vector Nn, bxdfParameters *parameters) const;
        
        void frameBegin();
        void frameEnd();
        
	private:
		Color color;
	};
}

#endif
