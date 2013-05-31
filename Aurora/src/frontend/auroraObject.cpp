//
//  auroraObject.cpp
//  Aurora
//
//  Created by Espen Nordahl on 01/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "auroraObject.h"
#include "renderableGeometry.h"
#include "shadingEngine.h"
#include "log.h"
#define lcontext LOG_AuroraObj

#include <iostream>

using namespace Aurora;

AuroraObject::AuroraObject(std::string name, RenderEnvironment *renderEnv, Reference<Shape> _shape, Material * _material ) :
FrontEndObject(name, renderEnv), 
shape(_shape), material(_material) {
	
}

AuroraObject::AuroraObject(std::string name, RenderEnvironment *renderEnv, Reference<Shape> _shape, std::string _matName ):
FrontEndObject(name, renderEnv),
shape(_shape), matName(_matName)
{
    
}
void AuroraObject::frameBegin(){
    LOG_DEBUG("Assigning material " << matName << " for object: " << name);
    material = renderEnv->shadingEngine->getMaterial(matName);
    material->frameBegin();
}

void AuroraObject::frameEnd(){
    material->frameEnd();
}

BBox AuroraObject::worldBound(){
	return shape->worldBound();
}

BBox AuroraObject::objectBound(){
	return shape->objectBound();
}

void AuroraObject::dice( std::vector<Reference<Geometry> > &diced){
	shape->dice(diced);
}

void AuroraObject::makeRenderable( std::vector<RenderableTriangle> &renderable, AttributeState *attrs, int index){
        // dice the shape into triangles
	std::vector<Reference<Geometry> > diced;
	dice(diced);
	for (int i=0; i < diced.size(); i++) {
		diced[i]->makeRenderable(renderable, index);
	}
    
        // set up attribute state
	attrs[index].material = material;
	attrs[index].emmision = Color(0);
        // TODO: Support all transforms
//    attrs[index].cameraToWorld = shape->cameraToWorld;
//    attrs[index].worldToCamera = shape->worldToCamera;
//    attrs[index].objectToWorld = shape->objectToWorld;
//    attrs[index].worldToObject = shape->worldToObject;
    attrs[index].cameraToObject = shape->cameraToObject;
    attrs[index].objectToCamera = shape->objectToCamera;

}