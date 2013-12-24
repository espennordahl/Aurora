//
//  AttributeChange.h
//  Aurora
//
//  Created by Espen Nordahl on 17/09/13.
//  Copyright (c) 2013 Espen Nordahl. All rights reserved.
//

#ifndef __Aurora__AttributeChange__
#define __Aurora__AttributeChange__

#include <iostream>
#include <string>

namespace Aurora {
    class AttributeChange{
    public:
        enum ValueType{
            kNullValue,
            kStringValue,
            kFloatValue
        };
        
        enum ObjectType{
            kShaderChange,
            kShapeChange,
            kLightChange
        };
    
        AttributeChange(const std::string &objectName, const std::string &attributeName, float floatValue, ObjectType objecttype);
        AttributeChange(const std::string &objectName, const std::string &attributeName, std::string stringValue, ObjectType objecttype);
        
        std::string objectName() const;
        std::string attributeName() const;
        
        float floatValue() const;
        std::string stringValue() const;
    
        ObjectType objectType() const;
        
    private:
        std::string m_objectname;
        std::string m_attributename;

        ValueType m_valuetype;
        float m_floatvalue;
        std::string m_stringvalue;
        
        ObjectType m_objecttype;
    };
}

#endif /* defined(__Aurora__AttributeChange__) */
