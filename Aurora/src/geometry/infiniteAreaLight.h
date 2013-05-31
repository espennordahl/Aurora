//
//  infiniteAreaLight.h
//  Aurora
//
//  Created by Espen Nordahl on 09/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_infiniteAreaLight_h
#define Aurora_infiniteAreaLight_h

#include "light.h"
#include "core.h"
#include "texture2D.h"

#include <string.h>

namespace Aurora {
    class InfiniteAreaLight : public Light {
	public:
		InfiniteAreaLight( Transform *o2c, Transform *c2o, Transform *o2w, Transform *w2o, Transform *c2w, Transform *w2c, float exposure, Color color, std::string envmap, std::string name, RenderEnvironment *renderEnv);
		
		Sample3D generateSample( const Point &orig, const Vector &Nn, const IntegrationDomain &integrationDomain );
		
		Color eval( const Sample3D &sample, const Vector &Nn ) ;
		
		float pdf( Sample3D *sample, const Vector &Nn, const IntegrationDomain &integrationDomain) ;
		
		bool intersectBinary( Ray *ray ) const;
		
		void makeRenderable(std::vector<RenderableTriangle> &renderable, AttributeState *attrs, int index);
		
        Color emission(){return Color(1);};
        
        Reference<Shape> shape() { return NULL; };
        
        void frameBegin(){};
        void frameEnd(){};

        
	private:
		Texture2D *envMap;
        bool usingMap;
	};
}


#endif
