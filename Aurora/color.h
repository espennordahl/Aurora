//
//  color.h
//  Aurora
//
//	Abstract:
//	standard rgb Color that assumes linear space

#ifndef Color_h
#define Color_h

#include "vector.h"
#include "point.h"

namespace Aurora {
	class Color{
	public:
		float r;
		float g;
		float b;
		
		Color();
		Color(float _r, float _g, float _b);
		Color(float c);
		Color(const Vector &v);
		Color(const Point &p);
		
		Color operator+(const Color &col);
		Color &operator+=(const Color &col);
		Color operator+(float f);
		Color &operator+=(float f);
		
		Color operator-(const Color &col);
		Color &operator-=(const Color &col);
		Color operator-(float f);
		Color &operator-=(float f);
		
		Color operator*(const Color &col);
		Color &operator*=(const Color &col);
		Color operator*(float f);
		Color &operator*=(float f);
		Color operator*(double d);
		Color &operator*=(double d);
		
		Color operator/(const Color &col);
		Color &operator/=(const Color &col);
		Color operator/(float f);
		Color &operator/=(float f);
		
        float &operator[](int index);
		const float operator[](int index) const;
        
		bool hasNaNs();
		
		bool isBlack();
		
		float lum();
		
		friend std::ostream& operator<<(std::ostream &os, Color const &c){
			os << "(" << c.r << ", " << c.g << ", " << c.b << ")";
			return os;
		};

	};
}


#endif
