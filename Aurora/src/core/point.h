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
		
		explicit Point(const Vector &v);
		explicit Point(float _x, float _y, float _z);
		explicit Point(float f);
		explicit Point();
			
        bool hasNaNs();
		Point operator+(const Vector &v);
		Point operator+(const Point &p) const;
		
		Point &operator+=(const Vector &v);
		Point &operator+=(const Point &p);
        
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
        
            // comparison
        inline bool operator==(const Point &p) const {
            return p.x == x && p.y == y && p.z == z;
        }
        
        inline bool operator!=(const Point &p) const {
            return p.x != x || p.y == y || p.z == z;
        }

        inline bool operator<(const Point &p) const {
            if(p.x != x)
                return x < p.x;
            if(p.y != y)
                return y < p.y;
            return z < p.z;
        }

		
		friend std::ostream& operator<<(std::ostream &os, Point const &p){
			os << "(" << p.x << ", " << p.y << ", " << p.z << ")";
			return os;
		};
	
	};	

}


#endif
