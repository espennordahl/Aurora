//
//  kelemenMaterial.h
//  Aurora
//
//  Created by Espen Nordahl on 23/07/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_kelemenMaterial_h
#define Aurora_kelemenMaterial_h

#include "material.h"
#include "shader.h"
#include "texture2D.h"

namespace Aurora {
    
        // forward decleration
    class RenderEnvironment;
    
	class KelemenMaterial : public Material {
	public:
		KelemenMaterial( std::string name, RenderEnvironment *renderEnv,
                        int diffColIndex, int specColIndex,
                        int roughnessIndex, float reflectance,
                        int normalIndex);
		BrdfState getBrdf( const Vector &Vn, const Vector &Nn, const ShadingGeometry &shdGeo , bool mattePath) ;
		
        void frameBegin();
        void frameEnd();

        
	private:
        void preCalcAlbedo();
        float getAlbedo(float costheta, float roughness);
        
        
		Brdf * diffBrdf;
		Brdf * specBrdf;
        std::vector<std::vector<float> > albedoTable;
        float reflectance;
        float specGain;
        float diffGain;
        float avg;
        
        int roughnessIndex;
        int diffColorIndex;
        int specColorIndex;
        int reflectanceIndex;
	};
}

#endif
