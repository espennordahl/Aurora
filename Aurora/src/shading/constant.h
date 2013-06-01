//
//  constant.h
//  Aurora
//
//  Created by Espen Nordahl on 26/12/12.
//
//

#ifndef Aurora_constant_h
#define Aurora_constant_h

#include "core.h"
#include "brdf.h"

namespace Aurora {
    
	class Constant : public Brdf{
	public:
		Constant(std::string name, Color col, RenderEnvironment *renderEnv);
		
		Sample3D getSample(const Vector &Vn, const Vector &Nn, const LambertParameters &parameters) const;
		Color evalSampleTangent(const Vector &Ln, const Vector &Vn, const LambertParameters &parameters) const;
		Color evalSampleWorld(const Vector &Ln, const Vector &Vn, const Vector &Nn, const LambertParameters &parameters)const;
		
        void frameBegin();
        void frameEnd();
        float pdf(const Vector &Ln, const Vector &Vn, const Vector Nn) const;

    private:
        Color col;
	};
}


#endif
