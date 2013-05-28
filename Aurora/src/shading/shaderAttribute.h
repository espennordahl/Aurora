//
//  shaderAttribute.h
//  Aurora
//
//  Created by Espen Nordahl on 5/11/12.
//
//

#ifndef Aurora_shaderAttribute_h
#define Aurora_shaderAttribute_h

#include "color.h"
#include "point.h"
#include "vector.h"

#include <string.h>

namespace Aurora {
    
    enum valueType{
        boolAttr,
        floatAttr,
        colorAttr,
        pointAttr,
        vectorAttr,
        stringAttr
    };
    
    struct valueField{
        bool bool_;
        float float_;
        Color color_;
        Point point_;
        Vector vector_;
        std::string s_;
        valueType type;
    };
    
    class ShaderAttribute{
    public:
        ShaderAttribute(float f);
        ShaderAttribute(Color c);
        ShaderAttribute(std::string s);
        ShaderAttribute(Point p);
        ShaderAttribute(Vector v);
        ShaderAttribute(bool b);
        
        bool isType(const valueType &type) const;
        valueType getType() const;

        Color getColor() const;
        bool isColor() const;
        
        std::string getString() const;
        bool isString() const;
        
        float getFloat() const;
        bool isFloat() const;
        
        Point getPoint() const;
        bool isPoint() const;
        
        Vector getVector() const;
        bool isVector() const;
        
        bool getBool() const;
        bool isBool() const;
        
    private:
        valueField m_value;
    };
}

#endif
