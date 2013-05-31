//
//  squareLight.h
//  Aurora
//
//  Created by Espen Nordahl on 05/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_squareLight_h
#define Aurora_squareLight_h

#include <tbb/atomic.h>

#include "halton_sampler.h"
#include "core.h"
#include "light.h"
#include "attributeState.h"

namespace Aurora {
	class SquareLight : public Light {
	public:
		SquareLight( Transform *o2c, Transform *c2o, Transform *o2w, Transform *w2o, Transform *c2w, Transform *w2c, float exposure, Color color, float sizeX, float sizeY, std::string name, RenderEnvironment *renderEnv);
		
		Sample3D generateSample( const Point &orig, const Vector &Nn, const IntegrationDomain &integrationDomain);
		
		Color eval( const Sample3D &sample, const Vector &Nn ) ;
		
        float pdf( Sample3D *sample, const Vector &Nn, const IntegrationDomain &integrationDomain);
		
		bool intersectBinary( Ray *ray ) const;
		
		void makeRenderable(std::vector<RenderableTriangle> &renderable, AttributeState *attrs, int index);

        Color emission();
        
        Reference<Shape> shape();
        
        void frameBegin(){};
        void frameEnd(){};
        
	private:
		float xScale;
		float yScale;
        Vector lightN;
        Halton_sampler m_sampler;
        
        static tbb::atomic<int> m_halton_index;

	};
}


#endif
