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

AuroraObject::AuroraObject(std::string name, RenderEnvironment *renderEnv, std::tr1::shared_ptr<Shape> shape, Material * material ) :
FrontEndObject(name, renderEnv), 
m_shape(shape), m_material(material) {
	
}

AuroraObject::AuroraObject(std::string name, RenderEnvironment *renderEnv, std::tr1::shared_ptr<Shape> shape, std::string matName ):
FrontEndObject(name, renderEnv),
m_shape(shape), m_matName(matName)
{
    
}
void AuroraObject::frameBegin(){
    LOG_DEBUG("Assigning material " << m_matName << " for object: " << m_name);
    m_material = m_renderEnv->shadingEngine->getMaterial(m_matName);
    m_material->frameBegin();
}

void AuroraObject::frameEnd(){
    m_material->frameEnd();
}

BBox AuroraObject::worldBound(){
	return m_shape->worldBound();
}

BBox AuroraObject::objectBound(){
	return m_shape->objectBound();
}

void AuroraObject::applyAttributeChange(const AttributeChange &change){
    switch (change.objectType()) {
        case AttributeChange::kShaderChange:
            m_material->applyAttributeChange(change);
            break;
        case AttributeChange::kShapeChange:
            m_shape->applyAttributeChange(change);
            break;
        default:
            assert(false);
    }
}
