//
//  metalMaterial.h
//  Aurora
//
//  Created by Espen Nordahl on 7/06/13.
//  Copyright (c) 2013 Espen Nordahl. All rights reserved.
//

#ifndef __Aurora__metalMaterial__
#define __Aurora__metalMaterial__

#include <iostream>

#include "material.h"
#include "cookTorrance.h"

namespace Aurora {
	class MetalMaterial : public Material {
	public:
		MetalMaterial( std::string name, RenderEnvironment *renderEnv,
                      int reflectanceIndex, int roughnessIndexA,
                      int roughnessIndexB, int mixIndex, int gainIndex, int normalIndex);
		BrdfState getBrdf( const Vector &Vn, const Vector &Nn, const ShadingGeometry &shdGeo, bool mattePath ) ;
        
        void frameBegin();
        void frameEnd();
        
	private:
		Brdf * m_brdf;
        int m_reflectanceIndex;
        int m_roughnessIndexA;
        int m_roughnessIndexB;
        int m_mixIndex;
        int m_gainIndex;
	};
}


#endif /* defined(__Aurora__metalMaterial__) */
