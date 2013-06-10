//
//  color.h
//  Aurora
//
//	Abstract:
//	standard rgb Color that assumes linear space

#ifndef Color_h
#define Color_h

#include <assert.h>
#include <math.h>

#include "constants.h"

#include "vector.h"
#include "point.h"

namespace Aurora {
	class Color{
	public:
		float r;
		float g;
		float b;
		
#pragma mark -
#pragma mark Constructors
        
        Color(){
            r=0; g=0; b=0;
            assert(!hasNaNs());
        }
        Color(float _r, float _g, float _b):
        r(_r), g(_g), b(_b)
        {
            assert(!hasNaNs());
        }
        Color(float c):
        r(c),g(c),b(c)
        {
            assert(!hasNaNs());
        }
        
        Color(const Vector &v):
        r(v.x),g(v.y),b(v.z)
        {
            assert(!hasNaNs());
        }
        
        Color(const Point &p):
        r(p.x),g(p.y),b(p.z)
        {
            assert(!hasNaNs());
        }
        
#pragma mark -
#pragma mark Operators        
            // Addition
        inline const Color operator+(const Color &col)const{
            return Color(r+col.r, g+col.g, b+col.b);
        }
        inline Color &operator+=(const Color &col){
            r += col.r; g += col.g; b += col.b;
            return *this;
        }
        inline const Color operator+(float f)const{
            return Color(r+f, g+f, b+f);
        }
        inline Color &operator+=(float f){
            r += f; g += f; b += f;
            return *this;
        }
        
            // Subtraction
        inline const Color operator-(const Color &col)const{
            return Color(r+col.r, g+col.g, b+col.b);
        }
        inline Color &operator-=(const Color &col){
            r -= col.r; g -= col.g; b -= col.b;
            return *this;
        }
        inline const Color operator-(float f)const{
            return Color(r-f, g-f, b-f);
        }
        inline Color &operator-=(float f){
            r -= f; g -= f; b -= f;
            return *this;
        }
        
            // Multiplication
        inline const Color operator*(const Color &col) const{
            return Color(r*col.r, g*col.g, b*col.b);
        }
        inline Color &operator*=(const Color &col){
            r *= col.r; g *= col.g; b *= col.b;
            return *this;
        }
        inline const Color operator*(float f) const{
            return Color(r*f, g*f, b*f);
        }
        inline Color &operator*=(float f){
            r *= f; g *= f; b *= f;
            return *this;
        }
        inline const Color operator*(double d) const{
            return Color(r*d, g*d, b*d);
        }
        inline Color &operator*=(double d){
            r *= d; g *= d; b *= d;
            return *this;
        }
        
            // Division
        inline const Color operator/(const Color &col)const{
            return Color(r/col.r, g/col.g, b/col.b);
        }
        inline Color &operator/=(const Color &col){
            r /= col.r; g /= col.g; b /= col.b;
            return *this;
        }
        inline const Color operator/(float f)const{
            float d = 1/f;
            return Color(r*d, g*d, b*d);
        }
        inline Color &operator/=(float f){
            float d = 1/f;
            r *= d; g *= d; b *= d;
            return *this;
        }
        
        inline float &operator[](int index){
            if (index == 0)
                return r;
            else if (index == 1)
                return g;
            else
                return b;
        }
        
        inline const float operator[](int index) const{
            if (index == 0)
                return r;
            else if (index == 1)
                return g;
            else
                return b;
        }
        
        inline bool isBlack(){
            return (r < ISBLACK && g < ISBLACK && b < ISBLACK);
        }
        
        inline bool hasNaNs(){
            return (isnan(r) || isnan(g) || isnan(b));
        }
        
        inline float lum(){
            return 0.2126*r + 0.7152*g + 0.0722*b; 
        }
        
        inline void clamp(float f){
            r = fminf(f, r);
            g = fminf(f, g);
            b = fminf(f, b);
        }
        
		friend std::ostream& operator<<(std::ostream &os, Color const &c){
			os << "(" << c.r << ", " << c.g << ", " << c.b << ")";
			return os;
		};

	};
}


#endif
