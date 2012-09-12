//
//  light.h
//  Aurora
//
//  Created by Espen Nordahl on 05/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_light_h
#define Aurora_light_h

#include "core.h"
#include "renderableTriangle.h"
#include "attributeState.h"

namespace Aurora {
    
    enum LightType{
        type_envLight,
        type_areaLight
    };
    
	class Light : public ReferenceCounted {
	public:
		Light( Transform *o2c, Transform *c2o, Transform *o2w, Transform *w2o, Transform *c2w, Transform *w2c, float exposure, Color color, int numSamples);
		
		virtual Sample3D generateSample( const Point &orig, const Vector &Nn, const IntegrationDomain &integrationDomain, int depth, int thread ) = 0;
		
		virtual Color eval( const Sample3D &sample, const Vector &Nn ) = 0;
		
		virtual float pdf( Sample3D *sample, const Vector &Nn, const IntegrationDomain &integrationDomain ) = 0;
		
		virtual bool intersectBinary( Ray *ray ) const = 0;
		
		virtual void makeRenderable(std::vector<RenderableTriangle> &renderable, AttributeState *attrs, int index)= 0;
	        
        virtual Color emission() = 0;
        
        virtual Reference<Shape> shape() = 0;
        
        LightType lightType;
        
	protected:
		Transform *objectToCamera;
		Transform *cameraToObject;
		Transform *objectToWorld;
		Transform *worldToObject;
		Transform *cameraToWorld;
		Transform *worldToCamera;
		
		float exposure;
		Color color;
        int numSamples;
	};
}

#endif
