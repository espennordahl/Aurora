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
#include "attributes.h"
#include "shape.h"
#include "material.h"
#include "renderableTriangle.h"
#include "attributeState.h"
#include "embreeMesh.h"

namespace Aurora {
	class AuroraObject : public ReferenceCounted {
	public:	
		AuroraObject(){ };
		AuroraObject( Reference<Shape> shape, Reference<Material> material );
		
		BBox worldBound();
		BBox objectBound();
		
		void dice( std::vector<Reference<Geometry> > &diced);
		void makeRenderable(std::vector<RenderableTriangle> &renderable, AttributeState *attrs, int index);
                
		Reference <Material> material;
		Reference <Shape> shape;
		Reference <Attributes> attributes;
	};
}


#endif
