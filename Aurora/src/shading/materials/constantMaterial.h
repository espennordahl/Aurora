//
//  constantMaterial.h
//  Aurora
//
//  Created by Espen Nordahl on 26/12/12.
//
//

#ifndef Aurora_constantMaterial_h
#define Aurora_constantMaterial_h


#include "material.h"
#include "constant.h"

namespace Aurora {
	class ConstantMaterial : public Material {
	public:
		ConstantMaterial( std::string name, Color col, RenderEnvironment *renderEnv);
		Brdf * getBrdf( const Vector &Vn, const Vector &Nn, const ShadingGeometry &shdGeo ) ;
        LambertParameters getBrdfParameters(const ShadingGeometry &shdGeo, Brdf *brdf) { return LambertParameters();}
        void frameBegin();
        void frameEnd();
        
	private:
		Brdf * brdf;
	};
}

#endif
