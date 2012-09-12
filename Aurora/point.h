//
//  point.h
//  Aurora
//
//	Abstract:
//	Standard 3d Point.


#ifndef Point_h
#define Point_h

#include "vector.h"

#include <ostream>

namespace Aurora{
	
	class Point{
	public:
		float x;
		float y;
		float z;
		
		Point(const Vector &v);
		Point(float _x, float _y, float _z);
		Point(float f);
		Point();
			
		Point operator+(const Vector &v);
		Vector operator+(const Point &p) const;
		
		Point &operator+=(const Vector &v);

		Point operator-(const Vector &v);
		Point &operator-=(const Vector &v);

		Vector operator-(const Point &p) const;
		
		Point operator*(float f) const;
		Point &operator*=(float f);
	
		Point operator/(float f);
		Point operator/=(float f);
		
		Point operator-();

		float &operator[](int index);
		const float operator[](int index) const;
		
		friend std::ostream& operator<<(std::ostream &os, Point const &p){
			os << "(" << p.x << ", " << p.y << ", " << p.z << ")";
			return os;
		};
	
	};	

}


#endif
