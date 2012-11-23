//
//  shaderAttribute.cpp
//  Aurora
//
//  Created by Espen Nordahl on 6/11/12.
//
//

#include "shaderAttribute.h"

#include "log.h"
#define lcontext LOG_ShaderAttr

using namespace Aurora;

ShaderAttribute::ShaderAttribute(float f){
    m_value.float_ = f;
    m_value.type = floatAttr;
}
ShaderAttribute::ShaderAttribute(Color c){
    m_value.color_ = c;
    m_value.type = colorAttr;
}
ShaderAttribute::ShaderAttribute(std::string s){
    m_value.s_ = s;
    m_value.type = stringAttr;
}
ShaderAttribute::ShaderAttribute(Point p){
    m_value.point_ = p;
    m_value.type = pointAttr;
}
ShaderAttribute::ShaderAttribute(Vector v){
    m_value.vector_ = v;
    m_value.type = vectorAttr;
}
ShaderAttribute::ShaderAttribute(bool b){
    m_value.bool_ = b;
    m_value.type = boolAttr;
}


valueType ShaderAttribute::getType() const{
    return m_value.type;
}

bool ShaderAttribute::isType(const valueType &type) const{
    switch (type) {
        case colorAttr:
            return isColor();
        case floatAttr:
            return isFloat();
        case boolAttr:
            return isBool();
        case vectorAttr:
            return isVector();
        case pointAttr:
            return isPoint();
        case stringAttr:
            return isString();
        default:
            LOG_ERROR("Unknown attribute type:" << type);
            return false;
    }
}

Color ShaderAttribute::getColor() const{
    if (m_value.type == colorAttr)
        return m_value.color_;
    else
        LOG_ERROR("Requested invalid attr type. Is " << m_value.type << ". Requested Color.");
    return Color(0);
}

bool ShaderAttribute::isColor() const{
    return m_value.type == colorAttr;
}

std::string ShaderAttribute::getString() const{
    if (m_value.type == stringAttr)
        return m_value.s_;
    else
        LOG_ERROR("Requested invalid attr type. Is " << m_value.type << ". Requested string.");
    return string("");
}
bool ShaderAttribute::isString() const{
    return m_value.type == stringAttr;
}

float ShaderAttribute::getFloat() const{
    if (m_value.type == floatAttr)
        return m_value.float_;
    else
        LOG_ERROR("Requested invalid attr type. Is " << m_value.type << ". Requested Float.");
    return 0.;
}
bool ShaderAttribute::isFloat() const{
    return m_value.type == floatAttr;
}

Point ShaderAttribute::getPoint() const{
    if (m_value.type == pointAttr)
        return m_value.point_;
    else
        LOG_ERROR("Requested invalid attr type. Is " << m_value.type << ". Requested Point.");
    return Point(0);
}
bool ShaderAttribute::isPoint() const{
    return m_value.type == pointAttr;
}

Vector ShaderAttribute::getVector() const{
    if (m_value.type == vectorAttr)
        return m_value.vector_;
    else
        LOG_ERROR("Requested invalid attr type. Is " << m_value.type << ". Requested Vector.");
    return Vector(0.);
}
bool ShaderAttribute::isVector() const{
    return m_value.type == vectorAttr;
}

bool ShaderAttribute::getBool() const{
    if (m_value.type == boolAttr)
        return m_value.bool_;
    else
        LOG_ERROR("Requested invalid attr type. Is " << m_value.type << ". Requested bool.");
    return false;
}
bool ShaderAttribute::isBool() const{
    return m_value.type == boolAttr;
}
