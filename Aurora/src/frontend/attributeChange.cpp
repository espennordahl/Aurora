//
//  AttributeChange.cpp
//  Aurora
//
//  Created by Espen Nordahl on 17/09/13.
//  Copyright (c) 2013 Espen Nordahl. All rights reserved.
//

#include "AttributeChange.h"

#include <assert.h>

using namespace Aurora;

AttributeChange::AttributeChange(const std::string &objectName, const std::string &attributeName, float floatValue, ObjectType objecttype):
m_objectname(objectName),
m_attributename(attributeName),
m_floatvalue(floatValue),
m_objecttype(objecttype),
m_valuetype(Aurora::AttributeChange::kFloatValue)
{}

AttributeChange::AttributeChange(const std::string &objectName, const std::string &attributeName, std::string stringValue, ObjectType objecttype):
m_objectname(objectName),
m_attributename(attributeName),
m_stringvalue(stringValue),
m_objecttype(objecttype),
m_valuetype(Aurora::AttributeChange::kStringValue)
{}

std::string AttributeChange::objectName() const{
    return m_objectname;
}


std::string AttributeChange::attributeName() const{
    return m_attributename;
}

float AttributeChange::floatValue() const{
    assert(m_valuetype == AttributeChange::kFloatValue);
    return m_floatvalue;
}

std::string AttributeChange::stringValue() const{
    assert(m_valuetype == AttributeChange::kStringValue);
    return m_stringvalue;
}

AttributeChange::ObjectType AttributeChange::objectType() const{
    return m_objecttype;
}
