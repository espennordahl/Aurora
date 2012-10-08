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
#include "kelemenShader.h"

namespace Aurora {
    
        // forward decleration
    class RenderEnvironment;
    
	class KelemenMaterial : public Material {
	public:
		KelemenMaterial( std::string name, RenderEnvironment *renderEnv, Color diffCol, Color specCol, 
                        float exponent, float reflectance, std::string diffTexture, 
                        bool useNoise, float noiseFrequency, int noiseLevels, float noiseAmplitudeMod, float noiseFreqMod,
                        const Color &noiseColA, const Color &noiseColB,
                        int numSamples);
		Reference<Brdf> getBrdf( const Vector &Vn, const Vector &Nn, const ShadingGeometry &shdGeo, int thread ) ;
		
        void frameBegin();
        void frameEnd();

        
	private:
        void preCalcAlbedo();
        float getAlbedo(float costheta);
		Reference<Brdf> diffBrdf;
		Reference<Brdf> specBrdf;
        std::vector<float> albedoTable;
        KelemenShader shader;
		float reflectance;
        float exponent;
        float specGain;
        float diffGain;
        float avg;
	};
}

#endif
