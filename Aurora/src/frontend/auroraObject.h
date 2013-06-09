//
//  auroraObject.h
//  Aurora
//
//  Created by Espen Nordahl on 01/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_auroraObject_h
#define Aurora_auroraObject_h

#include "core.h"
#include "shape.h"
#include "material.h"
#include "renderableTriangle.h"
#include "attributeState.h"
#include "embreeMesh.h"

namespace Aurora {
	class AuroraObject : public FrontEndObject {
	public:	
		AuroraObject(std::string name, RenderEnvironment *renderEnv): FrontEndObject(name, renderEnv) { };
		AuroraObject( std::string name, RenderEnvironment *renderEnv, std::tr1::shared_ptr<Shape> shape, Material * material );
		AuroraObject( std::string name, RenderEnvironment *renderEnv, std::tr1::shared_ptr<Shape> shape, std::string matName );

            // Calls the childrens methods of the same name
		BBox worldBound();
		BBox objectBound();
        
		    // FrameBegin and FrameEnd are pipeline methods for doing
            // any kind of pre and post rendering work. This method
            // is ran on all front end objects recursively before and
            // after rendering
        void frameBegin();
        void frameEnd();
        
            // During parsing we only store the material name.
        std::string m_matName;
            // The surface material to be used during light transport.
            // Only one can be used per object, but you can combine multiple
            // materials through the MaterialMixer material.
		Material *m_material;
            // Potentially un diced/-renderable geometry.
		shared_ptr<Shape> m_shape;
	};
}


#endif
