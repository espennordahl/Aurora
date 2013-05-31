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
	class AuroraObject : public FrontEndObject, public ReferenceCounted {
	public:	
		AuroraObject(std::string name, RenderEnvironment *renderEnv): FrontEndObject(name, renderEnv) { };
		AuroraObject( std::string name, RenderEnvironment *renderEnv, Reference<Shape> shape, Material * material );
		AuroraObject( std::string name, RenderEnvironment *renderEnv, Reference<Shape> shape, std::string matName );

            // Calls the childrens methods of the same name
		BBox worldBound();
		BBox objectBound();
        
		    // FrameBegin and FrameEnd are pipeline methods for doing
            // any kind of pre and post rendering work. This method
            // is ran on all front end objects recursively before and
            // after rendering
        void frameBegin();
        void frameEnd();

            // Splits the shape into triangles.
		void dice( std::vector<Reference<Geometry> > &diced);
        
            // Called once all objects have been parsed, and 
            // is responsible for initializing the correct AttributeState for the object
            // and convert all geometry to renderable triangles and add them to the 
            // passed vector.
		void makeRenderable(std::vector<RenderableTriangle> &renderable, AttributeState *attrs, int index);
                
            // During parsing we only store the material name.
        std::string matName;
            // The surface material to be used during light transport.
            // Only one can be used per object, but you can combine multiple
            // materials through the MaterialMixer material.
		Material *material;
            // Potentially un diced/-renderable geometry.
		Reference <Shape> shape;
	};
}


#endif
